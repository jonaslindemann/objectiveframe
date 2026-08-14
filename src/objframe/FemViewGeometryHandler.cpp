#include "FemViewGeometryHandler.h"

#include "FemView.h"
#include "FemViewModelGraph.h"

#include <ofmath/geom_ops.h>

#include <cmath>
#include <map>
#include <set>

using ofview_detail::applySelection;
using ofview_detail::buildGraph;
using ofview_detail::ModelGraph;

namespace {

constexpr double pi = 3.14159265358979323846;

// An array with a mistyped count would otherwise lock the render thread up for
// minutes building geometry nobody asked for.

constexpr size_t maxArrayNodes = 50000;

/**
 * Whether a support looks the same from every direction.
 *
 * The prescribed degrees of freedom are global, so a rotated copy of a roller
 * or of a prescribed displacement would restrain a different direction than the
 * original does. A whole triple that is either free or fixed at zero is the one
 * case where rotating changes nothing - which covers a full fixity and a pinned
 * support, the two that actually turn up.
 */
bool bcIsRotationInvariant(ofem::NodeBC *bc)
{
    auto prescribed = bc->getPrescribedArr();
    auto values = bc->getPrescribedValueArr();

    for (int triple = 0; triple < 2; triple++)
    {
        const int base = triple * 3;
        int count = 0;

        for (int i = 0; i < 3; i++)
        {
            if (!prescribed[base + i])
                continue;

            if (std::abs(values[base + i]) > 1e-12)
                return false;

            count++;
        }

        if ((count != 0) && (count != 3))
            return false;
    }

    return true;
}

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

void FemViewGeometryHandler::TransformContext::clear()
{
    nodes.clear();
    baseline.clear();
    adjacency.clear();
    affected.clear();
    hasBC.clear();
    hasLoad.clear();
}

bool FemViewGeometryHandler::TransformContext::empty() const
{
    return nodes.empty();
}

bool FemViewGeometryHandler::captureContext(FemViewWindow &view, const std::string &what, TransformContext &ctx)
{
    ctx.clear();

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

    std::set<ofem::Node *> selected = graph.selectedNodes;

    for (auto beam : graph.selectedBeams)
    {
        selected.insert(beam->getNode(0));
        selected.insert(beam->getNode(1));
    }

    // Capture every node, not only the selected ones. Smoothing needs the
    // neighbours just outside the selection to pull on the ones inside it,
    // and model order keeps the indices meaning what they mean everywhere
    // else in the application.

    std::map<ofem::Node *, int> indexOf;

    ctx.nodes = graph.nodes;
    ctx.baseline.reserve(ctx.nodes.size() * 3);
    ctx.adjacency.resize(ctx.nodes.size());
    ctx.affected.resize(ctx.nodes.size());
    ctx.hasBC.resize(ctx.nodes.size());
    ctx.hasLoad.resize(ctx.nodes.size());

    for (int i = 0; i < int(ctx.nodes.size()); i++)
    {
        auto node = ctx.nodes[i];

        indexOf[node] = i;

        double x, y, z;
        node->getCoord(x, y, z);

        ctx.baseline.push_back(x);
        ctx.baseline.push_back(y);
        ctx.baseline.push_back(z);

        ctx.affected[i] = (selected.count(node) > 0);
        ctx.hasBC[i] = hasAnyBC(view, node);
        ctx.hasLoad[i] = hasAnyLoad(view, node);
    }

    for (int i = 0; i < int(ctx.nodes.size()); i++)
    {
        for (auto beam : graph.beamsAt[ctx.nodes[i]])
        {
            ofem::Node *other = nullptr;
            ofview_detail::otherEnd(beam, ctx.nodes[i], other);

            if (other == nullptr)
                continue;

            auto it = indexOf.find(other);

            if (it != indexOf.end())
                ctx.adjacency[i].push_back(it->second);
        }
    }

    return true;
}

size_t FemViewGeometryHandler::movableFor(FemViewWindow &view, const TransformContext &ctx, const PinPolicy &pins,
                                          const std::string &what, bool report, std::vector<bool> &movable)
{
    movable.assign(ctx.nodes.size(), false);

    size_t count = 0;
    size_t pinned = 0;

    for (size_t i = 0; i < ctx.nodes.size(); i++)
    {
        if (!ctx.affected[i])
            continue;

        if ((pins.bcNodes && ctx.hasBC[i]) || (pins.loadedNodes && ctx.hasLoad[i]))
        {
            pinned++;
            continue;
        }

        movable[i] = true;
        count++;
    }

    if (report)
    {
        if (count == 0)
        {
            view.console(what + ": every selected node is pinned.");
            view.notify("Every selected node is pinned.", ofui::NotificationLevel::Warning);
        }
        else if (pinned > 0)
            view.console(what + ": " + std::to_string(pinned) + " pinned node(s) held in place.");
    }

    return count;
}

std::vector<glm::dvec3> FemViewGeometryHandler::baselinePoints(const TransformContext &ctx)
{
    std::vector<glm::dvec3> pts;
    pts.reserve(ctx.nodes.size());

    for (size_t i = 0; i < ctx.nodes.size(); i++)
        pts.emplace_back(ctx.baseline[i * 3], ctx.baseline[i * 3 + 1], ctx.baseline[i * 3 + 2]);

    return pts;
}

std::vector<glm::dvec3> FemViewGeometryHandler::movablePoints(const TransformContext &ctx,
                                                              const std::vector<bool> &movable)
{
    std::vector<glm::dvec3> pts;

    for (size_t i = 0; i < ctx.nodes.size(); i++)
        if (movable[i])
            pts.emplace_back(ctx.baseline[i * 3], ctx.baseline[i * 3 + 1], ctx.baseline[i * 3 + 2]);

    return pts;
}

void FemViewGeometryHandler::writeMovable(const TransformContext &ctx, const std::vector<bool> &movable,
                                          const std::vector<glm::dvec3> &pts)
{
    for (size_t i = 0; i < ctx.nodes.size(); i++)
        if (movable[i])
            ctx.nodes[i]->setCoord(pts[i].x, pts[i].y, pts[i].z);
}

void FemViewGeometryHandler::dropDisplacementMode(FemViewWindow &view)
{
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
}

void FemViewGeometryHandler::begin(FemViewWindow &view)
{
    view.snapShot();
    dropDisplacementMode(view);
}

void FemViewGeometryHandler::commit(FemViewWindow &view, const std::string &what, size_t moved)
{
    // One refresh at the end rather than per node - this updates the beams as
    // well, which is what keeps them from going stale when their ends move.

    view.refreshBeamModelVisuals();
    view.set_changed();
    view.m_solver.needRecalc = true;
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

bool FemViewGeometryHandler::applyTo(const TransformContext &ctx, const TransformParams &params,
                                     const std::vector<bool> &movable, const glm::dvec3 &origin,
                                     std::vector<glm::dvec3> &pts)
{
    pts = baselinePoints(ctx);

    // Smoothing is the one operation that reads its neighbours, so it works on
    // the whole point set and lets the movable mask decide who actually moves.
    // The matrix transforms touch only the movable points.

    if (params.kind == TransformKind::Smooth)
    {
        if (params.iterations <= 0)
            return false;

        ofmath::taubinSmooth(pts, ctx.adjacency, movable, params.iterations, params.lambda, params.mu,
                             params.lengthWeighted);

        return true;
    }

    glm::dmat4 m(1.0);

    switch (params.kind)
    {
    case TransformKind::Translate:
        m = ofmath::translationMatrix(glm::dvec3(params.delta[0], params.delta[1], params.delta[2]));
        break;

    case TransformKind::Scale:
        // A zero factor collapses the selection onto a plane and cannot be
        // undone by scaling back, so refuse it rather than quietly destroying
        // the geometry.

        for (int i = 0; i < 3; i++)
            if (std::abs(params.factors[i]) < 1e-12)
                return false;

        m = ofmath::scaleMatrix(glm::dvec3(params.factors[0], params.factors[1], params.factors[2]), origin);
        break;

    case TransformKind::Rotate: {
        glm::dvec3 axis(params.axis[0], params.axis[1], params.axis[2]);

        if (glm::length(axis) < 1e-12)
            return false;

        m = ofmath::rotationMatrix(axis, params.angleDeg * pi / 180.0, origin);
        break;
    }

    case TransformKind::Taper: {
        if ((params.taperAxis < 0) || (params.taperAxis > 2))
            return false;

        // The taper interpolates over the extent of what it is given, so it
        // must see the movable points alone rather than the whole model.

        auto subset = movablePoints(ctx, movable);
        ofmath::applyTaper(subset, params.taperAxis, params.s0, params.s1, origin);

        size_t k = 0;

        for (size_t i = 0; i < pts.size(); i++)
            if (movable[i])
                pts[i] = subset[k++];

        return true;
    }

    default:
        return false;
    }

    for (size_t i = 0; i < pts.size(); i++)
        if (movable[i])
            pts[i] = ofmath::transformPoint(m, pts[i]);

    return true;
}

void FemViewGeometryHandler::runOnce(FemViewWindow &view, const std::string &what, const TransformParams &params)
{
    TransformContext ctx;

    if (!captureContext(view, what, ctx))
        return;

    std::vector<bool> movable;

    size_t count = movableFor(view, ctx, params.pins, what, true, movable);

    if (count == 0)
        return;

    // Validate before taking a snapshot, so a refused command leaves no undo
    // entry behind.

    std::vector<glm::dvec3> pts;

    if (!applyTo(ctx, params, movable, originFor(view, params.origin, movablePoints(ctx, movable)), pts))
    {
        view.console(what + ": the parameters are degenerate and would collapse the selection.");
        view.notify(what + ": invalid parameters.", ofui::NotificationLevel::Warning);
        return;
    }

    begin(view);

    writeMovable(ctx, movable, pts);
    commit(view, what, count);
}

void FemViewGeometryHandler::translate(FemViewWindow &view, double dx, double dy, double dz)
{
    TransformParams params;

    params.kind = TransformKind::Translate;
    params.delta[0] = dx;
    params.delta[1] = dy;
    params.delta[2] = dz;

    runOnce(view, "Translate", params);
}

void FemViewGeometryHandler::scale(FemViewWindow &view, double sx, double sy, double sz, Origin origin)
{
    TransformParams params;

    params.kind = TransformKind::Scale;
    params.origin = origin;
    params.factors[0] = sx;
    params.factors[1] = sy;
    params.factors[2] = sz;

    runOnce(view, "Scale", params);
}

void FemViewGeometryHandler::rotate(FemViewWindow &view, double ax, double ay, double az, double angleDeg,
                                    Origin origin)
{
    TransformParams params;

    params.kind = TransformKind::Rotate;
    params.origin = origin;
    params.axis[0] = ax;
    params.axis[1] = ay;
    params.axis[2] = az;
    params.angleDeg = angleDeg;

    runOnce(view, "Rotate", params);
}

void FemViewGeometryHandler::taper(FemViewWindow &view, int axis, double s0, double s1, Origin origin)
{
    TransformParams params;

    params.kind = TransformKind::Taper;
    params.origin = origin;
    params.taperAxis = axis;
    params.s0 = s0;
    params.s1 = s1;

    runOnce(view, "Taper", params);
}

void FemViewGeometryHandler::smooth(FemViewWindow &view, int iterations, double lambda, double mu, bool lengthWeighted,
                                    PinPolicy pins)
{
    TransformParams params;

    params.kind = TransformKind::Smooth;
    params.pins = pins;
    params.iterations = iterations;
    params.lambda = lambda;
    params.mu = mu;
    params.lengthWeighted = lengthWeighted;

    runOnce(view, "Smooth", params);
}

bool FemViewGeometryHandler::previewActive(FemViewWindow &view)
{
    return view.m_geometry.previewActive;
}

bool FemViewGeometryHandler::beginPreview(FemViewWindow &view)
{
    // Starting a second session over a live one would capture the previewed
    // coordinates as the new baseline and bake the first transform in.

    if (view.m_geometry.previewActive)
        cancelPreview(view);

    if (!captureContext(view, "Transform", view.m_geometry.context))
    {
        view.m_geometry.context.clear();
        return false;
    }

    view.m_geometry.previewActive = true;

    dropDisplacementMode(view);

    return true;
}

void FemViewGeometryHandler::restoreBaseline(FemViewWindow &view)
{
    const auto &ctx = view.m_geometry.context;

    for (size_t i = 0; i < ctx.nodes.size(); i++)
        ctx.nodes[i]->setCoord(ctx.baseline[i * 3], ctx.baseline[i * 3 + 1], ctx.baseline[i * 3 + 2]);
}

void FemViewGeometryHandler::updatePreview(FemViewWindow &view, const TransformParams &params)
{
    auto &state = view.m_geometry;

    if (!state.previewActive)
        return;

    // The pin policy is re-derived on every update rather than baked into the
    // capture, so toggling a pin checkbox takes effect without recapturing.

    std::vector<bool> movable;
    std::vector<glm::dvec3> pts;

    size_t count = movableFor(view, state.context, params.pins, "Transform", false, movable);

    // Everything transforms from the captured coordinates, never from the
    // current ones - compounding would make the result depend on how the
    // slider was dragged. The origin comes from the baseline too, so it does
    // not drift as the preview moves the nodes around.

    if ((count == 0) || !applyTo(state.context, params, movable,
                                 originFor(view, params.origin, movablePoints(state.context, movable)), pts))
    {
        restoreBaseline(view);
        view.refreshBeamModelVisuals();
        view.redraw();
        return;
    }

    restoreBaseline(view);
    writeMovable(state.context, movable, pts);

    view.refreshBeamModelVisuals();
    view.redraw();
}

void FemViewGeometryHandler::applyPreview(FemViewWindow &view, const TransformParams &params)
{
    auto &state = view.m_geometry;

    if (!state.previewActive)
        return;

    std::vector<bool> movable;
    std::vector<glm::dvec3> pts;

    size_t count = movableFor(view, state.context, params.pins, "Transform", true, movable);

    // Put the model back before snapshotting, so undo returns to where the
    // gesture started rather than to the last previewed position.

    restoreBaseline(view);

    if ((count == 0) || !applyTo(state.context, params, movable,
                                 originFor(view, params.origin, movablePoints(state.context, movable)), pts))
    {
        cancelPreview(view);
        view.console("Transform: nothing to apply.");
        return;
    }

    begin(view);

    writeMovable(state.context, movable, pts);

    state.previewActive = false;
    state.context.clear();

    commit(view, "Transform", count);
}

void FemViewGeometryHandler::cancelPreview(FemViewWindow &view)
{
    auto &state = view.m_geometry;

    if (!state.previewActive)
        return;

    restoreBaseline(view);

    state.previewActive = false;
    state.context.clear();

    view.refreshBeamModelVisuals();
    view.redraw();
}

void FemViewGeometryHandler::collectCopySet(const ModelGraph &graph, std::vector<ofem::Node *> &nodes,
                                            std::vector<ofem::Beam *> &beams)
{
    nodes.clear();
    beams.clear();

    std::set<ofem::Node *> affected = graph.selectedNodes;

    for (auto beam : graph.selectedBeams)
    {
        affected.insert(beam->getNode(0));
        affected.insert(beam->getNode(1));
    }

    // Model order, so a copy is built in the same order as its original and the
    // new indices run in the same direction as the old ones.

    for (auto node : graph.nodes)
        if (affected.count(node) > 0)
            nodes.push_back(node);

    for (auto beam : graph.beams)
        if ((affected.count(beam->getNode(0)) > 0) && (affected.count(beam->getNode(1)) > 0))
            beams.push_back(beam);
}

void FemViewGeometryHandler::copyBeamProps(ofem::Beam *from, ofem::Beam *to)
{
    if ((from == nullptr) || (to == nullptr))
        return;

    // The cross section rotation matters as much as the material - a copy that
    // loses it draws a visibly different member.

    to->setMaterial(from->getMaterial());
    to->setBeamRotation(from->getBeamRotation());
    to->setBeamType(from->beamType());
    to->setEvaluationPoints(from->getEvaluationPoints());
}

void FemViewGeometryHandler::carryLoadsAndBCs(FemViewWindow &view, const std::map<ofem::Node *, ofem::Node *> &nodeCopy,
                                              const std::map<ofem::Beam *, ofem::Beam *> &beamCopy,
                                              const CopyOptions &opts, CopyReport &report)
{
    auto model = view.getModel();

    if (model == nullptr)
        return;

    auto bcSet = model->getBCSet();

    if (bcSet != nullptr)
    {
        for (long i = 0; i < long(bcSet->getSize()); i++)
        {
            auto bc = static_cast<ofem::NodeBC *>(bcSet->getBC(i));

            if (bc == nullptr)
                continue;

            // Collected before anything is added, so the copies this command
            // makes are not themselves picked up as sources.

            std::vector<ofem::Node *> targets;

            for (auto &entry : nodeCopy)
                if (bc->contains(entry.first))
                    targets.push_back(entry.second);

            if (targets.empty())
                continue;

            if (!opts.directionsPreserved && !bcIsRotationInvariant(bc))
            {
                report.skippedBCs += targets.size();
                continue;
            }

            for (auto node : targets)
                bc->addNode(node);
        }
    }

    auto loadSet = model->getNodeLoadSet();

    if (loadSet != nullptr)
    {
        for (long i = 0; i < long(loadSet->getSize()); i++)
        {
            auto load = static_cast<ofem::NodeLoad *>(loadSet->getLoad(i));

            if (load == nullptr)
                continue;

            std::vector<ofem::Node *> targets;

            for (unsigned int k = 0; k < load->getNodeSize(); k++)
            {
                auto it = nodeCopy.find(load->getNode(k));

                if (it != nodeCopy.end())
                    targets.push_back(it->second);
            }

            if (targets.empty())
                continue;

            // A node load points somewhere in global coordinates. Rotating the
            // copy without rotating the load would pull it in a direction
            // nobody asked for, so leave it off and say so.

            if (!opts.directionsPreserved)
            {
                report.skippedLoads += targets.size();
                continue;
            }

            for (auto node : targets)
                load->addNode(node);
        }
    }

    auto elementLoadSet = model->getElementLoadSet();

    if (elementLoadSet != nullptr)
    {
        for (long i = 0; i < long(elementLoadSet->getSize()); i++)
        {
            auto load = static_cast<ofem::ElementLoad *>(elementLoadSet->getLoad(i));

            if (load == nullptr)
                continue;

            std::vector<ofem::Element *> targets;

            for (unsigned int k = 0; k < load->getElementsSize(); k++)
            {
                auto it = beamCopy.find(static_cast<ofem::Beam *>(load->getElement(k)));

                if (it != beamCopy.end())
                    targets.push_back(it->second);
            }

            // A beam load is given in the member's own frame, so it follows the
            // copy wherever it ends up - no direction check needed.

            for (auto element : targets)
                load->addElement(element);
        }
    }
}

void FemViewGeometryHandler::duplicateOnce(FemViewWindow &view, const std::vector<ofem::Node *> &nodes,
                                           const std::vector<ofem::Beam *> &beams, const glm::dmat4 &m,
                                           const CopyOptions &opts, bool checkExisting, CopyReport &report)
{
    auto nodeSet = view.getModel()->getNodeSet();

    // addNode() offsets by the last picked position, which is what makes the
    // create gesture land under the cursor. These coordinates are absolute, so
    // zero it for the duration - the same thing the script runner does.

    double savedPos[3] = {view.m_selectedPos[0], view.m_selectedPos[1], view.m_selectedPos[2]};

    view.m_selectedPos[0] = 0.0;
    view.m_selectedPos[1] = 0.0;
    view.m_selectedPos[2] = 0.0;

    std::map<ofem::Node *, int> copyIndex;
    std::map<ofem::Node *, ofem::Node *> nodeCopy;

    for (auto node : nodes)
    {
        auto p = ofmath::transformPoint(m, coordOf(node));

        view.addNode(p.x, p.y, p.z);

        int index = int(nodeSet->getSize()) - 1;
        auto copy = nodeSet->getNode(index);

        copyIndex[node] = index;
        nodeCopy[node] = copy;

        report.nodes.push_back(copy);
    }

    view.m_selectedPos[0] = savedPos[0];
    view.m_selectedPos[1] = savedPos[1];
    view.m_selectedPos[2] = savedPos[2];

    std::map<ofem::Beam *, ofem::Beam *> beamCopy;

    for (auto beam : beams)
    {
        auto it0 = copyIndex.find(beam->getNode(0));
        auto it1 = copyIndex.find(beam->getNode(1));

        if ((it0 == copyIndex.end()) || (it1 == copyIndex.end()))
            continue;

        auto visBeam =
            checkExisting ? view.addBeam(it0->second, it1->second) : view.addBeamUnchecked(it0->second, it1->second);

        // Null when that pair is already connected, which happens for a beam
        // lying in the mirror plane.

        if (visBeam == nullptr)
            continue;

        copyBeamProps(beam, visBeam->getBeam());

        beamCopy[beam] = visBeam->getBeam();
        report.beams.push_back(visBeam->getBeam());
    }

    if (opts.loadsAndBCs)
        carryLoadsAndBCs(view, nodeCopy, beamCopy, opts, report);
}

void FemViewGeometryHandler::mirror(FemViewWindow &view, int axis, Origin origin, double weldTolerance)
{
    if ((axis < 0) || (axis > 2))
        return;

    // An un-applied preview is not part of the model yet, and it holds raw node
    // pointers. Cancelling it first means the copy is made from the real
    // geometry, and that a later cancel cannot write to a node the weld deleted.

    cancelPreview(view);

    ModelGraph graph;

    if (!buildGraph(view, graph))
        return;

    if (!graph.hasSelection())
    {
        view.console("Mirror: nothing is selected.");
        view.notify("Nothing is selected.", ofui::NotificationLevel::Warning);
        return;
    }

    std::vector<ofem::Node *> nodes;
    std::vector<ofem::Beam *> beams;

    collectCopySet(graph, nodes, beams);

    if (nodes.empty())
        return;

    glm::dvec3 normal(0.0);
    normal[axis] = 1.0;

    auto m = ofmath::mirrorMatrix(originFor(view, origin, coordsOf(nodes)), normal);

    begin(view);

    // Geometry and materials only. A prescribed displacement or a load vector
    // has a direction, and silently reflecting one would change the load case
    // without saying so.

    CopyOptions opts;
    opts.loadsAndBCs = false;

    CopyReport report;

    duplicateOnce(view, nodes, beams, m, opts, true, report);

    // The no-snapshot weld, so a mirror with a weld is still one undo entry.

    if (weldTolerance > 0.0)
        view.weldNearNodes(weldTolerance);

    view.refreshBeamModelVisuals();
    view.set_changed();
    view.m_solver.needRecalc = true;
    view.redraw();

    view.console("Mirror: " + std::to_string(report.nodes.size()) + " node(s) and " +
                 std::to_string(report.beams.size()) +
                 " beam(s) copied. Boundary conditions and loads were not copied.");
    view.notify("Mirrored " + std::to_string(report.nodes.size()) + " nodes. BCs and loads not copied.",
                ofui::NotificationLevel::Info);
}

void FemViewGeometryHandler::array(FemViewWindow &view, const ArrayParams &params)
{
    const std::string what = (params.kind == ArrayKind::Linear)
                                 ? "Array"
                                 : ((params.kind == ArrayKind::Polar) ? "Polar array" : "Grid array");

    // The counts include the original, so a grid is only empty when both
    // directions repeat once - a 1 x 4 grid is a perfectly good request.

    const bool isGrid = (params.kind == ArrayKind::Grid);
    const int count2 = isGrid ? params.count2 : 1;

    if ((params.count < 1) || (count2 < 1))
    {
        view.console(what + ": a repeat count below one is meaningless.");
        view.notify("Array needs at least two instances.", ofui::NotificationLevel::Warning);
        return;
    }

    const int instances = params.count * count2;

    if (instances < 2)
    {
        view.console(what + ": " + std::to_string(instances) + " instance(s) adds nothing.");
        view.notify("Array needs at least two instances.", ofui::NotificationLevel::Warning);
        return;
    }

    // See mirror() - an un-applied preview holds raw node pointers that the
    // weld below could delete.

    cancelPreview(view);

    ModelGraph graph;

    if (!buildGraph(view, graph))
        return;

    if (!graph.hasSelection())
    {
        view.console(what + ": nothing is selected.");
        view.notify("Nothing is selected.", ofui::NotificationLevel::Warning);
        return;
    }

    std::vector<ofem::Node *> nodes;
    std::vector<ofem::Beam *> beams;

    collectCopySet(graph, nodes, beams);

    if (nodes.empty())
        return;

    const size_t newNodes = nodes.size() * size_t(instances - 1);

    if (newNodes > maxArrayNodes)
    {
        view.console(what + ": " + std::to_string(newNodes) + " new nodes exceeds the limit of " +
                     std::to_string(maxArrayNodes) + ".");
        view.notify("Too many copies - reduce the count or the selection.", ofui::NotificationLevel::Warning);
        return;
    }

    // Each copy is built from the original by a single transform rather than
    // from the copy before it, so the last instance is as exact as the first.
    //
    // Everything is validated before begin(), so a refused array leaves no undo
    // entry behind - the same rule runOnce() follows.

    std::vector<glm::dmat4> steps;

    if (params.kind != ArrayKind::Polar)
    {
        // A grid is the linear case with a second direction, so both go through
        // the same code - one command, one snapshot, one weld, rather than the
        // two undo entries that chaining two linear arrays would cost.

        glm::dvec3 extent(1.0);

        if (params.spanStep)
        {
            // Measured over the copy set, so a step of one puts each copy
            // exactly one selection length on. A selection that is flat along
            // a step direction gives a zero offset and is refused below, which
            // is the honest answer - there is no length to repeat.

            glm::dvec3 lo(0.0), hi(0.0);
            ofmath::boundingBox(coordsOf(nodes), lo, hi);

            extent = hi - lo;
        }

        glm::dvec3 offset1 = glm::dvec3(params.offset[0], params.offset[1], params.offset[2]) * extent;
        glm::dvec3 offset2 =
            isGrid ? glm::dvec3(params.offset2[0], params.offset2[1], params.offset2[2]) * extent : glm::dvec3(0.0);

        // Only a direction that actually repeats has to go anywhere - a 1 x 4
        // grid is allowed to leave the unused step at zero.

        if ((params.count > 1) && (glm::length(offset1) < 1e-12))
        {
            view.console(what + ": the step is zero, every copy would land on the original.");
            view.notify("Array step is zero.", ofui::NotificationLevel::Warning);
            return;
        }

        if (isGrid && (params.count2 > 1) && (glm::length(offset2) < 1e-12))
        {
            view.console(what + ": the second step is zero, every copy would land on the original.");
            view.notify("Array step is zero.", ofui::NotificationLevel::Warning);
            return;
        }

        // Parallel directions collapse the grid onto a line, where the copies
        // of one row land on top of another's.

        if (isGrid && (params.count > 1) && (params.count2 > 1) &&
            (glm::length(glm::cross(offset1, offset2)) < 1e-12 * glm::length(offset1) * glm::length(offset2)))
        {
            view.console(what + ": the two directions are parallel, the rows would land on each other.");
            view.notify("Grid directions are parallel.", ofui::NotificationLevel::Warning);
            return;
        }

        for (int i = 0; i < params.count; i++)
            for (int j = 0; j < count2; j++)
            {
                if ((i == 0) && (j == 0))
                    continue;

                steps.push_back(ofmath::translationMatrix(offset1 * double(i) + offset2 * double(j)));
            }
    }
    else
    {
        glm::dvec3 axis(params.axis[0], params.axis[1], params.axis[2]);

        if (glm::length(axis) < 1e-12)
        {
            view.console(what + ": the axis has zero length.");
            view.notify("Array axis is zero.", ofui::NotificationLevel::Warning);
            return;
        }

        // A full circle divides by the count, so the last copy stops one step
        // short of the original instead of on top of it. A partial sweep
        // divides by the gaps, so the first and last instance sit on the ends
        // of the arc.

        const double stepDeg = params.fullCircle ? params.totalAngleDeg / double(params.count)
                                                 : params.totalAngleDeg / double(params.count - 1);

        if (std::abs(std::fmod(stepDeg, 360.0)) < 1e-9)
        {
            view.console(what + ": a step of " + std::to_string(stepDeg) + " degrees puts every copy on the original.");
            view.notify("Array angle gives a zero step.", ofui::NotificationLevel::Warning);
            return;
        }

        auto pts = coordsOf(nodes);
        auto centre = originFor(view, params.origin, pts);
        auto anchor = ofmath::centroid(pts);

        for (int k = 1; k < params.count; k++)
        {
            auto r = ofmath::rotationMatrix(axis, double(k) * stepDeg * pi / 180.0, centre);

            if (params.rotateCopies)
            {
                steps.push_back(r);
                continue;
            }

            // The insertion point follows the arc while the copy keeps its
            // orientation. With the selection centred on the axis that leaves
            // every copy on top of the original, which is worth refusing rather
            // than silently producing coincident geometry.

            auto delta = ofmath::transformPoint(r, anchor) - anchor;

            if ((k == 1) && (glm::length(delta) < 1e-12))
            {
                view.console(what + ": unrotated copies of a selection centred on the axis land on the original.");
                view.notify("Nothing to array - the selection sits on the axis.", ofui::NotificationLevel::Warning);
                return;
            }

            steps.push_back(ofmath::translationMatrix(delta));
        }
    }

    begin(view);

    CopyOptions opts;

    opts.loadsAndBCs = params.copyLoadsAndBCs;
    // Only a rotation disturbs a global direction. Linear and grid arrays are
    // pure translations, and so is a polar array that does not turn its copies.

    opts.directionsPreserved = (params.kind != ArrayKind::Polar) || !params.rotateCopies;

    CopyReport report;

    // The duplicate check in addBeam() is skipped: within a copy the node pairs
    // are fresh, and the cases where a copy could land on the original were
    // refused above.

    for (const auto &m : steps)
        duplicateOnce(view, nodes, beams, m, opts, false, report);

    if (params.weldTolerance > 0.0)
        view.weldNearNodes(params.weldTolerance);

    view.refreshBeamModelVisuals();
    view.set_changed();
    view.m_solver.needRecalc = true;
    view.redraw();

    std::string message = what + ": " + std::to_string(instances - 1) + " copies, " +
                          std::to_string(report.nodes.size()) + " node(s) and " + std::to_string(report.beams.size()) +
                          " beam(s) added.";

    if (report.skippedBCs > 0)
        message += " " + std::to_string(report.skippedBCs) +
                   " boundary condition(s) not copied - a rotation does not preserve them.";

    if (report.skippedLoads > 0)
        message += " " + std::to_string(report.skippedLoads) + " node load(s) not copied - their direction is global.";

    // Leave the original and every copy selected, so a second array in another
    // direction turns one bay into a grid.
    //
    // Welding deletes nodes, so the survivors are read back out of the model
    // rather than trusted from the list built before the weld. The wanted sets
    // may hold addresses of deleted nodes, which are only ever looked up, never
    // followed - nothing is allocated in between, so no new object can alias
    // one of them.

    std::set<ofem::Node *> wantedNodes(nodes.begin(), nodes.end());
    std::set<ofem::Beam *> wantedBeams(beams.begin(), beams.end());

    wantedNodes.insert(report.nodes.begin(), report.nodes.end());
    wantedBeams.insert(report.beams.begin(), report.beams.end());

    ModelGraph after;

    if (buildGraph(view, after))
    {
        std::set<ofem::Node *> selectedNodes;
        std::set<ofem::Beam *> selectedBeams;

        for (auto node : after.nodes)
            if (wantedNodes.count(node) > 0)
                selectedNodes.insert(node);

        for (auto beam : after.beams)
            if (wantedBeams.count(beam) > 0)
                selectedBeams.insert(beam);

        applySelection(view, after, selectedNodes, selectedBeams);
    }

    view.console(message);
    view.notify(what + ": " + std::to_string(report.nodes.size()) + " nodes added.", ofui::NotificationLevel::Info);
}
