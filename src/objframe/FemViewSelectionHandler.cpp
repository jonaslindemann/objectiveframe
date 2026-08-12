#include "FemViewSelectionHandler.h"

#include "FemView.h"
#include "FemViewModelGraph.h"

#include <algorithm>
#include <cmath>
#include <set>
#include <string>
#include <vector>

using ofview_detail::applySelection;
using ofview_detail::beamDirection;
using ofview_detail::buildGraph;
using ofview_detail::ModelGraph;
using ofview_detail::nodeCoord;
using ofview_detail::otherEnd;

void FemViewSelectionHandler::report(FemViewWindow &view, const std::string what)
{
    view.console(what + ": " + std::to_string(view.selectionShapeCount("vfem::Node")) + " nodes, " +
                 std::to_string(view.selectionShapeCount("vfem::Beam")) + " beams selected.");
}

void FemViewSelectionHandler::growSelection(FemViewWindow &view)
{
    ModelGraph graph;

    if (!buildGraph(view, graph))
        return;

    if (!graph.hasSelection())
    {
        view.console("Grow selection: nothing is selected.");
        return;
    }

    auto nodes = graph.selectedNodes;
    auto beams = graph.selectedBeams;

    // Selected beams pull in their end nodes

    for (auto beam : graph.selectedBeams)
    {
        nodes.insert(beam->getNode(0));
        nodes.insert(beam->getNode(1));
    }

    // Selected nodes pull in the beams meeting there, and those beams pull in
    // the nodes at their far ends - one ring outwards.

    for (auto node : graph.selectedNodes)
    {
        for (auto beam : graph.beamsAt[node])
        {
            beams.insert(beam);
            nodes.insert(beam->getNode(0));
            nodes.insert(beam->getNode(1));
        }
    }

    applySelection(view, graph, nodes, beams);
    report(view, "Grow selection");
}

void FemViewSelectionHandler::shrinkSelection(FemViewWindow &view)
{
    ModelGraph graph;

    if (!buildGraph(view, graph))
        return;

    if (!graph.hasSelection())
    {
        view.console("Shrink selection: nothing is selected.");
        return;
    }

    // A node stays only if every neighbour it reaches is selected too. That
    // strips exactly the ring of nodes on the edge of the selection.

    std::set<ofem::Node *> nodes;

    for (auto node : graph.selectedNodes)
    {
        bool interior = true;

        for (auto beam : graph.beamsAt[node])
        {
            ofem::Node *other = nullptr;
            otherEnd(beam, node, other);

            if ((other != nullptr) && (graph.selectedNodes.count(other) == 0))
            {
                interior = false;
                break;
            }
        }

        if (interior)
            nodes.insert(node);
    }

    // A beam survives only if both its ends survived

    std::set<ofem::Beam *> beams;

    for (auto beam : graph.selectedBeams)
    {
        if ((nodes.count(beam->getNode(0)) > 0) && (nodes.count(beam->getNode(1)) > 0))
            beams.insert(beam);
    }

    applySelection(view, graph, nodes, beams);
    report(view, "Shrink selection");
}

void FemViewSelectionHandler::selectConnected(FemViewWindow &view)
{
    ModelGraph graph;

    if (!buildGraph(view, graph))
        return;

    if (!graph.hasSelection())
    {
        view.console("Select connected: nothing is selected.");
        return;
    }

    // Breadth first walk over the node graph from everything selected

    std::set<ofem::Node *> nodes = graph.selectedNodes;
    std::vector<ofem::Node *> pending(graph.selectedNodes.begin(), graph.selectedNodes.end());

    for (auto beam : graph.selectedBeams)
    {
        for (int i = 0; i < 2; i++)
        {
            auto node = beam->getNode(i);

            if (nodes.insert(node).second)
                pending.push_back(node);
        }
    }

    while (!pending.empty())
    {
        auto node = pending.back();
        pending.pop_back();

        for (auto beam : graph.beamsAt[node])
        {
            ofem::Node *other = nullptr;
            otherEnd(beam, node, other);

            if ((other != nullptr) && (nodes.insert(other).second))
                pending.push_back(other);
        }
    }

    // Every beam with both ends in the component belongs to it

    std::set<ofem::Beam *> beams;

    for (auto beam : graph.beams)
    {
        if ((nodes.count(beam->getNode(0)) > 0) && (nodes.count(beam->getNode(1)) > 0))
            beams.insert(beam);
    }

    applySelection(view, graph, nodes, beams);
    report(view, "Select connected");
}

void FemViewSelectionHandler::selectMember(FemViewWindow &view)
{
    ModelGraph graph;

    if (!buildGraph(view, graph))
        return;

    // Seed from selected beams only. Starting from a node would be ambiguous -
    // every member meeting at that joint would be equally valid.

    std::set<ofem::Beam *> seeds = graph.selectedBeams;

    if (seeds.empty())
    {
        view.console("Select member: select a beam first.");
        return;
    }

    // Two beams continue the same member when their directions line up. 5
    // degrees of slack absorbs the drift in a generated or imported model.

    const double collinearLimit = std::cos(5.0 * 3.14159265358979323846 / 180.0);

    std::set<ofem::Beam *> beams = seeds;
    std::vector<ofem::Beam *> pending(seeds.begin(), seeds.end());

    while (!pending.empty())
    {
        auto beam = pending.back();
        pending.pop_back();

        double dir[3];

        if (!beamDirection(beam, dir))
            continue;

        for (int i = 0; i < 2; i++)
        {
            auto node = beam->getNode(i);

            // A joint where more than two beams meet ends the member

            if (graph.beamsAt[node].size() != 2)
                continue;

            for (auto next : graph.beamsAt[node])
            {
                if (next == beam)
                    continue;

                double nextDir[3];

                if (!beamDirection(next, nextDir))
                    continue;

                double dot = dir[0] * nextDir[0] + dir[1] * nextDir[1] + dir[2] * nextDir[2];

                // Either orientation is fine - the beams may be defined in
                // opposite directions along the same member.

                if (std::abs(dot) < collinearLimit)
                    continue;

                if (beams.insert(next).second)
                    pending.push_back(next);
            }
        }
    }

    std::set<ofem::Node *> nodes;

    for (auto beam : beams)
    {
        nodes.insert(beam->getNode(0));
        nodes.insert(beam->getNode(1));
    }

    applySelection(view, graph, nodes, beams);
    report(view, "Select member");
}

void FemViewSelectionHandler::selectSamePlane(FemViewWindow &view, int axis)
{
    if ((axis < 0) || (axis > 2))
        return;

    ModelGraph graph;

    if (!buildGraph(view, graph))
        return;

    // Seed from the selected nodes, plus the ends of any selected beam

    std::set<ofem::Node *> seeds = graph.selectedNodes;

    for (auto beam : graph.selectedBeams)
    {
        seeds.insert(beam->getNode(0));
        seeds.insert(beam->getNode(1));
    }

    if (seeds.empty())
    {
        view.console("Select same plane: select a node first.");
        return;
    }

    // Same tolerance rule as the ground node filter, so "level with this node"
    // means the same thing everywhere in the application.

    const double tolerance = std::max(view.getWorkspace() * 1.0e-4, 1.0e-5);

    std::vector<double> levels;

    for (auto node : seeds)
    {
        double v[3];
        nodeCoord(node, v);
        levels.push_back(v[axis]);
    }

    auto atLevel = [&](ofem::Node *node) {
        double v[3];
        nodeCoord(node, v);

        for (auto level : levels)
            if (std::abs(v[axis] - level) < tolerance)
                return true;

        return false;
    };

    std::set<ofem::Node *> nodes;

    for (auto node : graph.nodes)
        if (atLevel(node))
            nodes.insert(node);

    // Beams lying wholly in the plane come along, beams crossing it do not

    std::set<ofem::Beam *> beams;

    for (auto beam : graph.beams)
        if ((nodes.count(beam->getNode(0)) > 0) && (nodes.count(beam->getNode(1)) > 0))
            beams.insert(beam);

    const char *axisName = (axis == 0) ? "x" : ((axis == 1) ? "y" : "z");
    applySelection(view, graph, nodes, beams);
    report(view, "Select same " + std::string(axisName));
}

void FemViewSelectionHandler::selectSameMaterial(FemViewWindow &view)
{
    ModelGraph graph;

    if (!buildGraph(view, graph))
        return;

    if (graph.selectedBeams.empty())
    {
        view.console("Select same material: select a beam first.");
        return;
    }

    std::set<ofem::BeamMaterial *> materials;

    for (auto beam : graph.selectedBeams)
        materials.insert(beam->getMaterial());

    std::set<ofem::Beam *> beams;

    for (auto beam : graph.beams)
        if (materials.count(beam->getMaterial()) > 0)
            beams.insert(beam);

    std::set<ofem::Node *> nodes;

    applySelection(view, graph, nodes, beams);
    report(view, "Select same material");
}
