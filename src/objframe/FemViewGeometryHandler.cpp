#include "FemViewGeometryHandler.h"

#include "FemView.h"
#include "FemViewModelGraph.h"

#include <ofmath/geom_ops.h>

#include <cmath>
#include <map>
#include <set>

using ofview_detail::buildGraph;
using ofview_detail::ModelGraph;

namespace {

constexpr double pi = 3.14159265358979323846;

glm::dvec3 coordOf(ofem::Node *node)
{
    double x, y, z;
    node->getCoord(x, y, z);
    return glm::dvec3(x, y, z);
}

std::vector<glm::dvec3> coordsOf(const std::vector<ofem::Node *> &nodes)
{
    std::vector<glm::dvec3> pts;
    pts.reserve(nodes.size());

    for (auto node : nodes)
        pts.push_back(coordOf(node));

    return pts;
}

void writeCoords(const std::vector<ofem::Node *> &nodes, const std::vector<glm::dvec3> &pts)
{
    for (size_t i = 0; i < nodes.size(); i++)
        nodes[i]->setCoord(pts[i].x, pts[i].y, pts[i].z);
}

} // namespace

bool FemViewGeometryHandler::hasAnyBC(FemViewWindow &view, ofem::Node *node)
{
    auto model = view.getModel();

    if (model == nullptr)
        return false;

    auto bcSet = model->getBCSet();

    if (bcSet == nullptr)
        return false;

    for (long i = 0; i < long(bcSet->getSize()); i++)
    {
        auto bc = static_cast<ofem::NodeBC *>(bcSet->getBC(i));

        if ((bc != nullptr) && bc->contains(node))
            return true;
    }

    return false;
}

bool FemViewGeometryHandler::hasAnyLoad(FemViewWindow &view, ofem::Node *node)
{
    auto model = view.getModel();

    if (model == nullptr)
        return false;

    auto loadSet = model->getNodeLoadSet();

    if (loadSet == nullptr)
        return false;

    for (long i = 0; i < long(loadSet->getSize()); i++)
    {
        auto load = static_cast<ofem::BeamNodeLoad *>(loadSet->getLoad(i));

        if (load == nullptr)
            continue;

        for (unsigned int k = 0; k < load->getNodeSize(); k++)
            if (load->getNode(k) == node)
                return true;
    }

    return false;
}

bool FemViewGeometryHandler::gatherNodes(FemViewWindow &view, const std::string &what, const PinPolicy &pins,
                                         std::vector<ofem::Node *> &nodes)
{
    nodes.clear();

    ModelGraph graph;

    if (!buildGraph(view, graph))
        return false;

    if (!graph.hasSelection())
    {
        view.console(what + ": nothing is selected.");
        view.notify("Nothing is selected.", ofui::NotificationLevel::Warning);
        return false;
    }

    // Selected nodes, plus the ends of any selected beam

    std::set<ofem::Node *> affected = graph.selectedNodes;

    for (auto beam : graph.selectedBeams)
    {
        affected.insert(beam->getNode(0));
        affected.insert(beam->getNode(1));
    }

    size_t pinned = 0;

    // Walk the model order rather than the set order, so the resulting node
    // list is deterministic and matches what the rest of the application calls
    // node 0, node 1 and so on.

    for (auto node : graph.nodes)
    {
        if (affected.count(node) == 0)
            continue;

        if ((pins.bcNodes && hasAnyBC(view, node)) || (pins.loadedNodes && hasAnyLoad(view, node)))
        {
            pinned++;
            continue;
        }

        nodes.push_back(node);
    }

    if (nodes.empty())
    {
        view.console(what + ": every selected node is pinned.");
        view.notify("Every selected node is pinned.", ofui::NotificationLevel::Warning);
        return false;
    }

    if (pinned > 0)
        view.console(what + ": " + std::to_string(pinned) + " pinned node(s) held in place.");

    return true;
}

void FemViewGeometryHandler::begin(FemViewWindow &view)
{
    view.snapShot();

    // Results belong to the old geometry. Showing them over the new one would
    // draw nodes at their new coordinate plus their old scaled displacement.
    //
    // Only the displacement based modes have that problem. Fem and Geometry
    // both draw undeformed geometry, and they draw it differently - Fem as
    // solid tubes, Geometry as the real cross section - so switching between
    // them here would change how the model looks for no reason.

    auto repr = view.getRepresentation();

    if ((repr == RepresentationMode::Displacements) || (repr == RepresentationMode::Results))
        view.setRepresentation(RepresentationMode::Geometry);

    view.m_solver.needRecalc = true;
}

void FemViewGeometryHandler::commit(FemViewWindow &view, const std::string &what, size_t moved)
{
    // One refresh at the end rather than per node - this updates the beams as
    // well, which is what keeps them from going stale when their ends move.

    view.refreshBeamModelVisuals();
    view.set_changed();
    view.redraw();

    view.console(what + ": " + std::to_string(moved) + " node(s) moved.");
}

namespace {

// Resolves an Origin against the nodes a command is about to act on. The
// cursor position is passed in because only the handler itself can reach it.

glm::dvec3 resolveOrigin(FemViewGeometryHandler::Origin origin, const std::vector<glm::dvec3> &pts,
                         const glm::dvec3 &cursor)
{
    switch (origin)
    {
    case FemViewGeometryHandler::Origin::Centroid:
        return ofmath::centroid(pts);

    case FemViewGeometryHandler::Origin::BoundingBoxCentre: {
        glm::dvec3 lo(0.0), hi(0.0);
        ofmath::boundingBox(pts, lo, hi);
        return 0.5 * (lo + hi);
    }

    case FemViewGeometryHandler::Origin::BoundingBoxLow: {
        glm::dvec3 lo(0.0), hi(0.0);
        ofmath::boundingBox(pts, lo, hi);
        return lo;
    }

    case FemViewGeometryHandler::Origin::BoundingBoxHigh: {
        glm::dvec3 lo(0.0), hi(0.0);
        ofmath::boundingBox(pts, lo, hi);
        return hi;
    }

    case FemViewGeometryHandler::Origin::Cursor:
        return cursor;

    case FemViewGeometryHandler::Origin::World:
    default:
        return glm::dvec3(0.0);
    }
}

} // namespace

glm::dvec3 FemViewGeometryHandler::originFor(FemViewWindow &view, Origin origin, const std::vector<glm::dvec3> &pts)
{
    return resolveOrigin(origin, pts, glm::dvec3(view.m_selectedPos[0], view.m_selectedPos[1], view.m_selectedPos[2]));
}

void FemViewGeometryHandler::translate(FemViewWindow &view, double dx, double dy, double dz)
{
    std::vector<ofem::Node *> nodes;

    if (!gatherNodes(view, "Translate", PinPolicy{}, nodes))
        return;

    begin(view);

    auto pts = coordsOf(nodes);
    auto m = ofmath::translationMatrix(glm::dvec3(dx, dy, dz));

    for (auto &p : pts)
        p = ofmath::transformPoint(m, p);

    writeCoords(nodes, pts);
    commit(view, "Translate", nodes.size());
}

void FemViewGeometryHandler::scale(FemViewWindow &view, double sx, double sy, double sz, Origin origin)
{
    std::vector<ofem::Node *> nodes;

    if (!gatherNodes(view, "Scale", PinPolicy{}, nodes))
        return;

    // A zero factor collapses the model onto a plane and cannot be undone by
    // scaling back, so refuse it rather than quietly destroying the geometry.

    if ((std::abs(sx) < 1e-12) || (std::abs(sy) < 1e-12) || (std::abs(sz) < 1e-12))
    {
        view.console("Scale: a scale factor of zero would collapse the selection.");
        view.notify("Scale factors must be non-zero.", ofui::NotificationLevel::Warning);
        return;
    }

    begin(view);

    auto pts = coordsOf(nodes);
    auto m = ofmath::scaleMatrix(glm::dvec3(sx, sy, sz), originFor(view, origin, pts));

    for (auto &p : pts)
        p = ofmath::transformPoint(m, p);

    writeCoords(nodes, pts);
    commit(view, "Scale", nodes.size());
}

void FemViewGeometryHandler::rotate(FemViewWindow &view, double ax, double ay, double az, double angleDeg,
                                    Origin origin)
{
    std::vector<ofem::Node *> nodes;

    if (!gatherNodes(view, "Rotate", PinPolicy{}, nodes))
        return;

    glm::dvec3 axis(ax, ay, az);

    if (glm::length(axis) < 1e-12)
    {
        view.console("Rotate: the rotation axis has zero length.");
        view.notify("The rotation axis must be non-zero.", ofui::NotificationLevel::Warning);
        return;
    }

    begin(view);

    auto pts = coordsOf(nodes);
    auto m = ofmath::rotationMatrix(axis, angleDeg * pi / 180.0, originFor(view, origin, pts));

    for (auto &p : pts)
        p = ofmath::transformPoint(m, p);

    writeCoords(nodes, pts);
    commit(view, "Rotate", nodes.size());
}

void FemViewGeometryHandler::taper(FemViewWindow &view, int axis, double s0, double s1, Origin origin)
{
    if ((axis < 0) || (axis > 2))
        return;

    std::vector<ofem::Node *> nodes;

    if (!gatherNodes(view, "Taper", PinPolicy{}, nodes))
        return;

    begin(view);

    auto pts = coordsOf(nodes);
    ofmath::applyTaper(pts, axis, s0, s1, originFor(view, origin, pts));

    writeCoords(nodes, pts);
    commit(view, "Taper", nodes.size());
}

void FemViewGeometryHandler::mirror(FemViewWindow &view, int axis, Origin origin, double weldTolerance)
{
    if ((axis < 0) || (axis > 2))
        return;

    ModelGraph graph;

    if (!buildGraph(view, graph))
        return;

    if (!graph.hasSelection())
    {
        view.console("Mirror: nothing is selected.");
        view.notify("Nothing is selected.", ofui::NotificationLevel::Warning);
        return;
    }

    // Nodes to copy, in model order

    std::set<ofem::Node *> affected = graph.selectedNodes;

    for (auto beam : graph.selectedBeams)
    {
        affected.insert(beam->getNode(0));
        affected.insert(beam->getNode(1));
    }

    std::vector<ofem::Node *> nodes;
    std::map<ofem::Node *, int> sourceIndex;

    for (int i = 0; i < int(graph.nodes.size()); i++)
    {
        if (affected.count(graph.nodes[i]) == 0)
            continue;

        sourceIndex[graph.nodes[i]] = i;
        nodes.push_back(graph.nodes[i]);
    }

    // Any beam with both ends in the copied set comes along. That covers both
    // gestures - selecting the beams and selecting the nodes they span.

    std::vector<ofem::Beam *> beams;

    for (auto beam : graph.beams)
        if ((affected.count(beam->getNode(0)) > 0) && (affected.count(beam->getNode(1)) > 0))
            beams.push_back(beam);

    auto pts = coordsOf(nodes);

    glm::dvec3 normal(0.0);
    normal[axis] = 1.0;

    auto m = ofmath::mirrorMatrix(originFor(view, origin, pts), normal);

    begin(view);

    // addNode() offsets by the last picked position, which is what makes the
    // create gesture land under the cursor. These coordinates are absolute, so
    // zero it for the duration - the same thing the script runner does.

    double savedPos[3] = {view.m_selectedPos[0], view.m_selectedPos[1], view.m_selectedPos[2]};

    view.m_selectedPos[0] = 0.0;
    view.m_selectedPos[1] = 0.0;
    view.m_selectedPos[2] = 0.0;

    auto nodeSet = view.getModel()->getNodeSet();
    std::map<int, int> mirroredIndex;

    for (size_t i = 0; i < nodes.size(); i++)
    {
        auto p = ofmath::transformPoint(m, pts[i]);

        view.addNode(p.x, p.y, p.z);
        mirroredIndex[sourceIndex[nodes[i]]] = int(nodeSet->getSize()) - 1;
    }

    view.m_selectedPos[0] = savedPos[0];
    view.m_selectedPos[1] = savedPos[1];
    view.m_selectedPos[2] = savedPos[2];

    size_t addedBeams = 0;

    for (auto beam : beams)
    {
        auto it0 = mirroredIndex.find(sourceIndex[beam->getNode(0)]);
        auto it1 = mirroredIndex.find(sourceIndex[beam->getNode(1)]);

        if ((it0 == mirroredIndex.end()) || (it1 == mirroredIndex.end()))
            continue;

        auto visBeam = view.addBeam(it0->second, it1->second);

        // addBeam() returns nullptr when that pair is already connected, which
        // happens for a beam lying in the mirror plane.

        if (visBeam == nullptr)
            continue;

        visBeam->getBeam()->setMaterial(beam->getMaterial());
        addedBeams++;
    }

    if (weldTolerance > 0.0)
        view.connectNearNodes(weldTolerance);

    view.refreshBeamModelVisuals();
    view.set_changed();
    view.redraw();

    view.console("Mirror: " + std::to_string(nodes.size()) + " node(s) and " + std::to_string(addedBeams) +
                 " beam(s) copied. Boundary conditions and loads were not copied.");
    view.notify("Mirrored " + std::to_string(nodes.size()) + " nodes. BCs and loads not copied.",
                ofui::NotificationLevel::Info);
}
