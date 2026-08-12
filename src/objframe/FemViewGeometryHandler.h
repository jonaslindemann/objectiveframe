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

    /** The transforms that move nodes in place, and so can be previewed. */
    enum class TransformKind {
        Translate,
        Scale,
        Rotate,
        Taper
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

        double delta[3]{0.0, 0.0, 0.0};   //!< Translate
        double factors[3]{1.0, 1.0, 1.0}; //!< Scale
        double axis[3]{0.0, 1.0, 0.0};    //!< Rotate
        double angleDeg{0.0};             //!< Rotate
        int taperAxis{1};                 //!< Taper
        double s0{1.0};                   //!< Taper, factor at the low end
        double s1{1.0};                   //!< Taper, factor at the high end
    };

    /**
     * Which nodes a command refuses to move.
     *
     * Transforms default to moving everything selected - an explicit transform
     * should do exactly what was asked. Smoothing and projection default to
     * pinning both, because relaxing a support or a load point changes what
     * the model means and does it invisibly.
     */
    struct PinPolicy {
        bool bcNodes{false};
        bool loadedNodes{false};
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
     * Applies params to a point cloud in place.
     *
     * Returns false for degenerate parameters - a zero scale factor or a zero
     * length rotation axis - leaving the points untouched.
     */
    static bool applyTo(std::vector<glm::dvec3> &pts, const TransformParams &params, const glm::dvec3 &origin);

    /** Runs params over the current selection as a single undoable command. */
    static void runOnce(FemViewWindow &view, const std::string &what, const TransformParams &params);

    /**
     * Collects the nodes a command should act on, honouring the pin policy.
     *
     * Returns false and reports on the console when the selection is empty or
     * everything in it is pinned.
     */
    static bool gatherNodes(FemViewWindow &view, const std::string &what, const PinPolicy &pins,
                            std::vector<ofem::Node *> &nodes);

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
