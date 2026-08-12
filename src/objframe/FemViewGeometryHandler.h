#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

namespace ofem {
class Node;
}

class FemViewWindow;

/**
 * Geometry modifying commands.
 *
 * These move the nodes of the current selection rather than rebuilding them,
 * so node and beam indices survive every operation. Scripts holding indices
 * and result arrays indexed by node stay valid - only mirror() adds anything,
 * and it appends.
 *
 * Every command takes a snapshot first, so all of them are undoable. A command
 * drops the view back to the geometry representation only when a displacement
 * based mode is showing, because moving nodes under a deformation would draw
 * the new geometry with the old displacements added on top.
 *
 * The nodes a command acts on are the selected nodes together with the end
 * nodes of the selected beams, so selecting a beam and scaling does what it
 * looks like it should.
 */
class FemViewGeometryHandler {
public:
    /**
     * What a transform is measured relative to.
     *
     * The two face origins exist for mirror(), where the useful plane is at
     * the edge of the selection rather than through it - mirroring about the
     * centre would reflect the selection onto itself.
     */
    enum class Origin {
        World,             //!< The world origin
        Centroid,          //!< Mean of the affected node positions
        BoundingBoxCentre, //!< Centre of the affected nodes' bounding box
        Cursor,            //!< The last picked position
        BoundingBoxLow,    //!< Low corner of the affected nodes' bounding box
        BoundingBoxHigh    //!< High corner of the affected nodes' bounding box
    };

    /** The operations that move nodes in place, and so can be previewed. */
    enum class TransformKind {
        Translate,
        Scale,
        Rotate,
        Taper,
        Smooth
    };

    /**
     * Which nodes a command refuses to move.
     *
     * Transforms default to moving everything selected - an explicit transform
     * should do exactly what was asked. Smoothing defaults to pinning both,
     * because relaxing a support or a load point changes what the model means
     * and does it invisibly.
     */
    struct PinPolicy {
        bool bcNodes{false};
        bool loadedNodes{false};
    };

    /**
     * A complete description of a node moving transform.
     *
     * One struct rather than an argument list per command, so the one shot
     * commands and the live preview drive exactly the same code and cannot
     * drift apart.
     */
    struct TransformParams {
        TransformKind kind{TransformKind::Translate};
        Origin origin{Origin::Centroid};
        PinPolicy pins;

        double delta[3]{0.0, 0.0, 0.0};   //!< Translate
        double factors[3]{1.0, 1.0, 1.0}; //!< Scale
        double axis[3]{0.0, 1.0, 0.0};    //!< Rotate
        double angleDeg{0.0};             //!< Rotate
        int taperAxis{1};                 //!< Taper
        double s0{1.0};                   //!< Taper, factor at the low end
        double s1{1.0};                   //!< Taper, factor at the high end

        int iterations{10};         //!< Smooth
        double lambda{0.5};         //!< Smooth, shrink factor
        double mu{-0.53};           //!< Smooth, inflate factor, 0 for plain Laplacian
        bool lengthWeighted{false}; //!< Smooth
    };

    /**
     * Everything an operation needs about the model, captured once.
     *
     * Holds *every* node rather than only the selected ones, because smoothing
     * has to see the neighbours just outside the selection - a node on the
     * boundary should be pulled towards the structure it is attached to, not
     * only towards its selected neighbours.
     *
     * `affected`, `hasBC` and `hasLoad` are stored separately from any movable
     * mask so that changing the pin policy re-derives which nodes move without
     * having to capture the model again.
     */
    struct TransformContext {
        std::vector<ofem::Node *> nodes; //!< Every model node, in model order
        std::vector<double> baseline;    //!< Three doubles per node
        std::vector<std::vector<int>> adjacency;

        std::vector<bool> affected; //!< In the current selection
        std::vector<bool> hasBC;
        std::vector<bool> hasLoad;

        void clear();
        bool empty() const;
    };

    // One shot commands

    static void translate(FemViewWindow &view, double dx, double dy, double dz);

    static void scale(FemViewWindow &view, double sx, double sy, double sz, Origin origin);

    /** Rotates by angleDeg about the axis (ax, ay, az) through origin. */
    static void rotate(FemViewWindow &view, double ax, double ay, double az, double angleDeg, Origin origin);

    /**
     * Scales perpendicular to an axis by a factor that varies along it.
     *
     * \param axis 0 for x, 1 for y, 2 for z
     * \param s0   factor at the low end of the selection
     * \param s1   factor at the high end
     */
    static void taper(FemViewWindow &view, int axis, double s0, double s1, Origin origin);

    /**
     * Taubin smoothing of the selected nodes.
     *
     * Nodes outside the selection are held fixed but still pull on their
     * selected neighbours, so smoothing part of a frame does not tear it away
     * from the rest.
     *
     * \param mu 0 runs plain Laplacian, which shrinks the structure towards
     *           its centroid - usually not what is wanted on a free standing
     *           frame. -0.53 against a lambda of 0.5 cancels most of it.
     */
    static void smooth(FemViewWindow &view, int iterations, double lambda, double mu, bool lengthWeighted,
                       PinPolicy pins);

    /**
     * Mirrors the selection in a principal plane and keeps both halves.
     *
     * \param axis          normal of the mirror plane, 0 for x, 1 for y, 2 for z
     * \param weldTolerance nodes closer than this are fused afterwards, which
     *                      joins the two halves along the seam. Pass 0 to skip
     *                      welding and leave the copy detached.
     *
     * Geometry and materials are copied. Boundary conditions and loads are
     * not - a prescribed displacement or a load vector has a direction, and
     * silently reflecting one would change the load case without saying so.
     *
     * Adds geometry rather than moving it, so it is not part of the preview
     * session below.
     */
    static void mirror(FemViewWindow &view, int axis, Origin origin, double weldTolerance);

    // Live preview session
    //
    // The panel captures the affected nodes once, then re-applies the
    // transform from those captured coordinates on every parameter change.
    // Applying from the baseline rather than compounding means the result
    // depends only on the current parameter values, never on the path the
    // sliders took, and one gesture produces exactly one undo entry.

    /** Captures the affected nodes and their coordinates. */
    static bool beginPreview(FemViewWindow &view);

    /** Restores the baseline and re-applies params. Cheap enough to call per frame. */
    static void updatePreview(FemViewWindow &view, const TransformParams &params);

    /** Makes the preview permanent, with a single snapshot for undo. */
    static void applyPreview(FemViewWindow &view, const TransformParams &params);

    /** Puts the nodes back where they were and ends the session. */
    static void cancelPreview(FemViewWindow &view);

    static bool previewActive(FemViewWindow &view);

private:
    /**
     * Applies params to the captured baseline, writing into pts.
     *
     * Returns false for degenerate parameters - a zero scale factor or a zero
     * length rotation axis - leaving the points untouched.
     */
    static bool applyTo(const TransformContext &ctx, const TransformParams &params, const std::vector<bool> &movable,
                        const glm::dvec3 &origin, std::vector<glm::dvec3> &pts);

    /** Runs params over the current selection as a single undoable command. */
    static void runOnce(FemViewWindow &view, const std::string &what, const TransformParams &params);

    /**
     * Captures the whole model plus which nodes are selected, fixed or loaded.
     *
     * Returns false and reports on the console when nothing is selected.
     */
    static bool captureContext(FemViewWindow &view, const std::string &what, TransformContext &ctx);

    /**
     * Derives which nodes may move under a pin policy.
     *
     * Returns the number of movable nodes, and reports how many were held.
     */
    static size_t movableFor(FemViewWindow &view, const TransformContext &ctx, const PinPolicy &pins,
                             const std::string &what, bool report, std::vector<bool> &movable);

    /** The captured coordinates, as points. */
    static std::vector<glm::dvec3> baselinePoints(const TransformContext &ctx);

    /** Just the movable points, which is what an origin should be measured over. */
    static std::vector<glm::dvec3> movablePoints(const TransformContext &ctx, const std::vector<bool> &movable);

    /** Writes pts onto the movable nodes only. */
    static void writeMovable(const TransformContext &ctx, const std::vector<bool> &movable,
                             const std::vector<glm::dvec3> &pts);

    /** Snapshot for undo, and drop out of a displacement representation. */
    static void begin(FemViewWindow &view);

    /**
     * Leaves a displacement representation, without taking a snapshot.
     *
     * `Fem` and `Geometry` are both undeformed and render differently, so
     * neither is disturbed.
     */
    static void dropDisplacementMode(FemViewWindow &view);

    /** Push the moved coordinates into the scene and report. */
    static void commit(FemViewWindow &view, const std::string &what, size_t moved);

    /** Writes the captured baseline coordinates back onto the preview nodes. */
    static void restoreBaseline(FemViewWindow &view);

    /**
     * Resolves an Origin against the points a command is about to transform.
     *
     * A member rather than a free function because Origin::Cursor reads the
     * last picked position out of the view.
     */
    static glm::dvec3 originFor(FemViewWindow &view, Origin origin, const std::vector<glm::dvec3> &pts);

    static bool hasAnyBC(FemViewWindow &view, ofem::Node *node);
    static bool hasAnyLoad(FemViewWindow &view, ofem::Node *node);
};
