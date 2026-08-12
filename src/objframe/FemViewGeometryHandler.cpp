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
