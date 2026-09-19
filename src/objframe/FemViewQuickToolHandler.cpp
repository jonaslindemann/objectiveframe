#include "FemViewQuickToolHandler.h"

#include "FemView.h"

#include <cmath>

bool FemViewQuickToolHandler::ForceSpec::valid() const
{
    if (std::abs(magnitude) < 1.0e-12)
        return false;

    return std::sqrt(dx * dx + dy * dy + dz * dz) > 1.0e-12;
}

vfem::Node *FemViewQuickToolHandler::asVisualNode(ivf::Shape *shape)
{
    if (shape == nullptr || !shape->isClass("vfem::Node"))
        return nullptr;

    return static_cast<vfem::Node *>(shape);
}

void FemViewQuickToolHandler::snapShotOnce(FemViewWindow &view)
{
    if (!view.m_quick.stampArmed)
        return;

    view.snapShot();
    view.m_quick.stampArmed = false;
}

void FemViewQuickToolHandler::armStamp(FemViewWindow &view)
{
    view.m_quick.stampArmed = true;
}

void FemViewQuickToolHandler::invalidateResults(FemViewWindow &view)
{
    view.m_solver.needRecalc = true;

    if (view.m_eigenmodeWindow != nullptr && view.m_eigenmodeWindow->hasEigenmodes())
        view.clearEigenmodes();

    view.set_changed();
    view.redraw();
}

ofem::BeamNodeLoad *FemViewQuickToolHandler::findLoad(FemViewWindow &view, const ForceSpec &spec)
{
    auto loadSet = view.m_beamModel->getNodeLoadSet();

    for (int i = 0; i < static_cast<int>(loadSet->getSize()); i++)
    {
        auto load = static_cast<ofem::BeamNodeLoad *>(loadSet->getLoad(i));

        if (load->matches(spec.dx, spec.dy, spec.dz, spec.magnitude))
            return load;
    }

    return nullptr;
}

ofem::BeamNodeLoad *FemViewQuickToolHandler::createLoad(FemViewWindow &view, const ForceSpec &spec)
{
    auto loadSet = view.m_beamModel->getNodeLoadSet();

    // The direction is normalised and the magnitude goes in the value, which is
    // the form the solver reads and the form matches() compares - a load
    // carrying its size in the direction vector would never coalesce with one
    // carrying it in the value.

    double length = std::sqrt(spec.dx * spec.dx + spec.dy * spec.dy + spec.dz * spec.dz);

    auto load = new ofem::BeamNodeLoad();
    load->setDirection(spec.dx / length, spec.dy / length, spec.dz / length);
    load->setValue(spec.magnitude);

    // The name is derived from what the load is, but the user is free to have
    // named something else the same thing already. Two identical entries in the
    // load list and the load mixer would be worse than a suffix.

    std::string baseName = ofem::BeamNodeLoad::autoName(spec.dx, spec.dy, spec.dz, spec.magnitude);
    std::string name = baseName;

    for (int suffix = 2; suffix < 1000; suffix++)
    {
        bool taken = false;

        for (int i = 0; i < static_cast<int>(loadSet->getSize()); i++)
            if (static_cast<ofem::BeamNodeLoad *>(loadSet->getLoad(i))->getName() == name)
            {
                taken = true;
                break;
            }

        if (!taken)
            break;

        name = baseName + " (" + std::to_string(suffix) + ")";
    }

    load->setName(name);

    loadSet->addLoad(load);

    // Gives the load its scene representation. Without this the arrows never
    // appear, however many nodes join the load.

    view.addNodeLoad(load);

    return load;
}

ofem::BeamNodeBC *FemViewQuickToolHandler::findBC(FemViewWindow &view, ConstraintKind kind)
{
    // The two oldest defaults are made with the model and held by it. Looking
    // them up by name as well would work, but going through the accessors keeps
    // this path and the older Edit menu commands on the same objects.

    if (kind == ConstraintKind::Fixed)
        return view.m_beamModel->defaultNodeFixedBC();

    if (kind == ConstraintKind::Pinned)
        return view.m_beamModel->defaultNodePosBC();

    auto bcSet = view.m_beamModel->getNodeBCSet();
    const std::string name = ofem::BeamNodeBC::defaultName(kind);

    for (int i = 0; i < static_cast<int>(bcSet->getSize()); i++)
    {
        auto bc = static_cast<ofem::BeamNodeBC *>(bcSet->getBC(i));

        if (bc->getName() == name)
            return bc;
    }

    return nullptr;
}

ofem::BeamNodeBC *FemViewQuickToolHandler::createBC(FemViewWindow &view, ConstraintKind kind)
{
    // Created on first use rather than with the model, so a model that never
    // uses a roller does not carry three empty BCs through every save. It is
    // found again by name on the next load, and readFromStream() restores the
    // read only flag from the same name table.

    auto bc = new ofem::BeamNodeBC();
    bc->applyDefault(kind);
    bc->setReadOnly();

    view.m_beamModel->getNodeBCSet()->addBC(bc);
    view.addNodeBC(bc);

    return bc;
}

bool FemViewQuickToolHandler::loadContains(ofem::BeamNodeLoad *load, ofem::Node *node)
{
    // ofem::NodeLoad has no membership test of its own, and the solver sums the
    // load over every entry - so a node added twice is silently loaded twice.

    for (int i = 0; i < static_cast<int>(load->getNodeSize()); i++)
        if (load->getNode(i) == node)
            return true;

    return false;
}

bool FemViewQuickToolHandler::nodeHasLoad(FemViewWindow &view, ofem::Node *node)
{
    auto loadSet = view.m_beamModel->getNodeLoadSet();

    for (int i = 0; i < static_cast<int>(loadSet->getSize()); i++)
        if (loadContains(static_cast<ofem::BeamNodeLoad *>(loadSet->getLoad(i)), node))
            return true;

    return false;
}

bool FemViewQuickToolHandler::nodeHasBC(FemViewWindow &view, ofem::Node *node)
{
    auto bcSet = view.m_beamModel->getNodeBCSet();

    for (int i = 0; i < static_cast<int>(bcSet->getSize()); i++)
        if (static_cast<ofem::BeamNodeBC *>(bcSet->getBC(i))->contains(node))
            return true;

    return false;
}

void FemViewQuickToolHandler::removeNodeFromAllLoads(FemViewWindow &view, ofem::Node *node)
{
    auto loadSet = view.m_beamModel->getNodeLoadSet();

    for (int i = 0; i < static_cast<int>(loadSet->getSize()); i++)
    {
        auto load = static_cast<ofem::BeamNodeLoad *>(loadSet->getLoad(i));

        // A node can only be in a load once if a quick tool put it there, but
        // looping costs nothing and covers loads built by other paths.

        while (load->removeNode(node))
            ;
    }
}

void FemViewQuickToolHandler::removeNodeFromAllBCs(FemViewWindow &view, ofem::Node *node)
{
    auto bcSet = view.m_beamModel->getNodeBCSet();

    // Read only BCs included, unlike removeNodeBCAt(). Read only means the BC
    // definition cannot be edited or deleted, not that the set of nodes using
    // it is fixed - and every standard support is read only, so skipping them
    // would leave a node in its old support forever.

    for (int i = 0; i < static_cast<int>(bcSet->getSize()); i++)
    {
        auto bc = static_cast<ofem::BeamNodeBC *>(bcSet->getBC(i));

        while (bc->removeNode(node))
            ;
    }
}

std::vector<ofem::Node *> FemViewQuickToolHandler::selectedNodes(FemViewWindow &view)
{
    std::vector<ofem::Node *> nodes;

    auto selected = view.getSelectedShapes();

    if (selected == nullptr)
        return nodes;

    for (int i = 0; i < selected->getSize(); i++)
    {
        auto visNode = asVisualNode(selected->getChild(i));

        if (visNode != nullptr)
            nodes.push_back(static_cast<ofem::Node *>(visNode->getFemNode()));
    }

    return nodes;
}

int FemViewQuickToolHandler::applyForceToSelection(FemViewWindow &view, const ForceSpec &spec)
{
    if (!spec.valid())
        return 0;

    auto nodes = selectedNodes(view);

    if (nodes.empty())
        return 0;

    // Work out what would change before anything does. A snapshot serialises
    // the whole model, and creating the load is itself a change - snapshotting
    // after it would make undo leave an empty load behind.

    auto load = findLoad(view, spec);

    std::vector<ofem::Node *> pending;

    for (auto node : nodes)
        if (load == nullptr || !loadContains(load, node))
            pending.push_back(node);

    if (pending.empty())
        return 0;

    view.snapShot();

    if (load == nullptr)
        load = createLoad(view, spec);

    for (auto node : pending)
        load->addNode(node);

    view.refreshBeamModelVisuals();
    invalidateResults(view);

    return static_cast<int>(pending.size());
}

bool FemViewQuickToolHandler::applyForceToNodeAt(FemViewWindow &view, int nodeIndex, const ForceSpec &spec)
{
    if (!spec.valid())
        return false;

    auto nodeSet = view.m_beamModel->getNodeSet();

    if (nodeIndex < 0 || nodeIndex >= static_cast<int>(nodeSet->getSize()))
        return false;

    auto node = nodeSet->getNode(nodeIndex);
    auto load = findLoad(view, spec);

    if (load != nullptr && loadContains(load, node))
        return false;

    // No snapshot, like every other per item "...At" mutator. These are the
    // scripting and REST entry points, where loads are added in loops and the
    // caller decides where the undo points go with its own snapShot() calls -
    // one full model serialisation per load would be both slow and unasked for.

    if (load == nullptr)
        load = createLoad(view, spec);

    load->addNode(node);

    auto visLoad = static_cast<vfem::NodeLoad *>(load->getUser());

    if (visLoad != nullptr)
        visLoad->refresh();

    invalidateResults(view);

    return true;
}

int FemViewQuickToolHandler::applyConstraintToSelection(FemViewWindow &view, ConstraintKind kind)
{
    auto nodes = selectedNodes(view);

    if (nodes.empty())
        return 0;

    auto bc = findBC(view, kind);

    std::vector<ofem::Node *> pending;

    for (auto node : nodes)
        if (bc == nullptr || !bc->contains(node))
            pending.push_back(node);

    if (pending.empty())
        return 0;

    view.snapShot();

    if (bc == nullptr)
        bc = createBC(view, kind);

    for (auto node : pending)
    {
        // One support per node - see the grouping note on the class. Without
        // this a node fixed after being pinned would sit in both BCs, which the
        // solver tolerates but the BC list reports as two supported nodes.

        removeNodeFromAllBCs(view, node);
        bc->addNode(node);
    }

    view.refreshBeamModelVisuals();
    invalidateResults(view);

    return static_cast<int>(pending.size());
}

int FemViewQuickToolHandler::clearForcesFromSelection(FemViewWindow &view)
{
    auto nodes = selectedNodes(view);

    std::vector<ofem::Node *> pending;

    for (auto node : nodes)
        if (nodeHasLoad(view, node))
            pending.push_back(node);

    if (pending.empty())
        return 0;

    view.snapShot();

    for (auto node : pending)
        removeNodeFromAllLoads(view, node);

    view.refreshBeamModelVisuals();
    invalidateResults(view);

    return static_cast<int>(pending.size());
}

int FemViewQuickToolHandler::clearConstraintsFromSelection(FemViewWindow &view)
{
    auto nodes = selectedNodes(view);

    std::vector<ofem::Node *> pending;

    for (auto node : nodes)
        if (nodeHasBC(view, node))
            pending.push_back(node);

    if (pending.empty())
        return 0;

    view.snapShot();

    for (auto node : pending)
        removeNodeFromAllBCs(view, node);

    view.refreshBeamModelVisuals();
    invalidateResults(view);

    return static_cast<int>(pending.size());
}

void FemViewQuickToolHandler::stampShape(FemViewWindow &view, ivf::Shape *shape, bool remove)
{
    auto visNode = asVisualNode(shape);

    if (visNode == nullptr)
        return;

    auto node = static_cast<ofem::Node *>(visNode->getFemNode());

    if (view.getEditMode() == WidgetMode::PaintLoad)
        stampLoad(view, node, remove);
    else
        stampConstraint(view, node, remove);
}

void FemViewQuickToolHandler::stampLoad(FemViewWindow &view, ofem::Node *node, bool remove)
{
    if (remove)
    {
        // Nothing to undo if the node carried no load, so the test comes first.

        if (!nodeHasLoad(view, node))
            return;

        snapShotOnce(view);
        removeNodeFromAllLoads(view, node);

        // Which load lost the node is not worth tracking here, so every load's
        // glyphs are rebuilt. Removal is the rarer half of the gesture.

        view.refreshBeamModelVisuals();
    }
    else
    {
        if (!view.m_quick.force.valid())
            return;

        auto load = findLoad(view, view.m_quick.force);

        if (load != nullptr && loadContains(load, node))
            return;

        snapShotOnce(view);

        if (load == nullptr)
            load = createLoad(view, view.m_quick.force);

        load->addNode(node);

        // Only the load that changed, so the arrow appears under the cursor as
        // the stroke passes without rebuilding the whole model.

        auto visLoad = static_cast<vfem::NodeLoad *>(load->getUser());

        if (visLoad != nullptr)
            visLoad->refresh();
    }

    invalidateResults(view);
}

void FemViewQuickToolHandler::stampConstraint(FemViewWindow &view, ofem::Node *node, bool remove)
{
    if (remove)
    {
        if (!nodeHasBC(view, node))
            return;

        snapShotOnce(view);
        removeNodeFromAllBCs(view, node);

        view.refreshBeamModelVisuals();
    }
    else
    {
        auto bc = findBC(view, view.m_quick.constraint);

        if (bc != nullptr && bc->contains(node))
            return;

        bool hadOther = nodeHasBC(view, node);

        snapShotOnce(view);

        if (bc == nullptr)
            bc = createBC(view, view.m_quick.constraint);

        removeNodeFromAllBCs(view, node);
        bc->addNode(node);

        // A node that was supported some other way costs a full refresh: the
        // glyph that has to disappear belongs to a different BC.

        if (hadOther)
            view.refreshBeamModelVisuals();
        else
        {
            auto visBC = static_cast<vfem::NodeBC *>(bc->getUser());

            if (visBC != nullptr)
                visBC->refresh();
        }
    }

    invalidateResults(view);
}

std::string FemViewQuickToolHandler::forceDescription(const ForceSpec &spec)
{
    if (!spec.valid())
        return "no force set";

    return ofem::BeamNodeLoad::autoName(spec.dx, spec.dy, spec.dz, spec.magnitude);
}

std::string FemViewQuickToolHandler::constraintDescription(ConstraintKind kind)
{
    return ofem::BeamNodeBC::defaultName(kind);
}
