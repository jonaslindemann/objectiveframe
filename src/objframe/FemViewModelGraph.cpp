#include "FemViewModelGraph.h"

#include "FemView.h"

#include <cmath>

namespace ofview_detail {

ofem::Beam *otherEnd(ofem::Beam *beam, ofem::Node *node, ofem::Node *&other)
{
    auto n0 = beam->getNode(0);
    auto n1 = beam->getNode(1);

    if (n0 == node)
        other = n1;
    else if (n1 == node)
        other = n0;
    else
        other = nullptr;

    return beam;
}

void nodeCoord(ofem::Node *node, double v[3])
{
    node->getCoord(v[0], v[1], v[2]);
}

bool beamDirection(ofem::Beam *beam, double dir[3])
{
    if (beam->getSize() < 2)
        return false;

    double a[3], b[3];
    nodeCoord(beam->getNode(0), a);
    nodeCoord(beam->getNode(1), b);

    dir[0] = b[0] - a[0];
    dir[1] = b[1] - a[1];
    dir[2] = b[2] - a[2];

    double length = std::sqrt(dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2]);

    if (length < 1e-12)
        return false;

    dir[0] /= length;
    dir[1] /= length;
    dir[2] /= length;

    return true;
}

bool buildGraph(FemViewWindow &view, ModelGraph &graph)
{
    auto model = view.getModel();

    if (model == nullptr)
        return false;

    auto nodeSet = model->getNodeSet();
    auto beamSet = model->getElementSet();

    if ((nodeSet == nullptr) || (beamSet == nullptr))
        return false;

    for (long i = 0; i < long(nodeSet->getSize()); i++)
        graph.nodes.push_back(nodeSet->getNode(i));

    for (long i = 0; i < long(beamSet->getSize()); i++)
    {
        auto beam = static_cast<ofem::Beam *>(beamSet->getElement(i));

        if (beam == nullptr || beam->getSize() < 2)
            continue;

        graph.beams.push_back(beam);
        graph.beamsAt[beam->getNode(0)].push_back(beam);
        graph.beamsAt[beam->getNode(1)].push_back(beam);
    }

    // Map model objects to scene shapes. Reading it from the scene rather than
    // from Base::getUser() keeps this honest about what is actually selectable.

    auto scene = view.getScene()->getComposite();

    for (int i = 0; i < scene->getSize(); i++)
    {
        auto shape = scene->getChild(i);

        if (shape->isClass("vfem::Node"))
        {
            auto visNode = static_cast<vfem::Node *>(shape);
            graph.nodeShape[visNode->getFemNode()] = shape;
        }
        else if (shape->isClass("vfem::Beam"))
        {
            auto visBeam = static_cast<vfem::Beam *>(shape);
            graph.beamShape[visBeam->getBeam()] = shape;
        }
    }

    auto selected = view.getSelectedShapes();

    for (int i = 0; i < selected->getSize(); i++)
    {
        auto shape = selected->getChild(i);

        if (shape->isClass("vfem::Node"))
            graph.selectedNodes.insert(static_cast<vfem::Node *>(shape)->getFemNode());
        else if (shape->isClass("vfem::Beam"))
            graph.selectedBeams.insert(static_cast<vfem::Beam *>(shape)->getBeam());
    }

    return true;
}

void applySelection(FemViewWindow &view, ModelGraph &graph, const std::set<ofem::Node *> &nodes,
                    const std::set<ofem::Beam *> &beams)
{
    std::vector<ivf::Shape *> shapes;

    for (auto node : nodes)
    {
        auto it = graph.nodeShape.find(node);
        if (it != graph.nodeShape.end())
            shapes.push_back(it->second);
    }

    for (auto beam : beams)
    {
        auto it = graph.beamShape.find(beam);
        if (it != graph.beamShape.end())
            shapes.push_back(it->second);
    }

    view.setSelection(shapes);
}

} // namespace ofview_detail
