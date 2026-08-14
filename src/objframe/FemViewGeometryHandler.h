#pragma once

#include <map>
#include <string>
#include <vector>

#include <glm/glm.hpp>

namespace ofem {
class Node;
class Beam;
} // namespace ofem

namespace ofview_detail {
struct ModelGraph;
}

class FemViewWindow;

/**
 * Geometry modifying commands.
 *
 * These move the nodes of the current selection rather than rebuilding them,
 * so node and beam indices survive every operation. Scripts holding indices
 * and result arrays indexed by node stay valid - only the duplicating commands,
 * mirror() and array(), add anything, and they append.
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

    /** How an array command lays its copies out. */
    enum class ArrayKind {
        Linear, //!< Repeated translation by a fixed step
        Polar,  //!< Repeated rotation about an axis
        Grid    //!< Repeated translation in two directions at once
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
     * A complete description of an array copy.
     *
     * `count` is the number of instances *including* the original, which is the
     * convention every CAD array command uses - a count of 1 is a no-op and a
     * count of 3 adds two copies.
     *
     * Loads and boundary conditions come along by default, which is the
     * opposite of mirror(). A linear array is a pure translation, so a load
     * vector and a prescribed direction mean exactly the same thing on the copy
     * as on the original. A polar array rotates, and array() then carries over
     * only what a rotation leaves alone - see carryLoadsAndBCs().
     */
    struct ArrayParams {
        ArrayKind kind{ArrayKind::Linear};
        int count{2};

        double offset[3]{1.0, 0.0, 0.0}; //!< Linear, the step per copy rather than the total span

        /**
         * Linear and Grid, measure the offsets in bounding box extents of the
         * selection.
         *
         * An offset of (1, 0, 0) then puts each copy exactly one selection
         * length further along x, which is what "repeat this bay" means and
         * what welding then joins into a continuous structure.
         */
        bool spanStep{false};

        // Grid, the second direction. The first one reuses count and offset
        // above, so a grid is a linear array that also repeats sideways - and
        // one command rather than two, which keeps it to a single snapshot and
        // a single weld.

        int count2{1};
        double offset2[3]{0.0, 0.0, 1.0};

        double axis[3]{0.0, 1.0, 0.0};   //!< Polar
        double totalAngleDeg{360.0};     //!< Polar, swept by the array as a whole
        bool fullCircle{true};           //!< Polar, step is total/count so the last copy meets the original
        Origin origin{Origin::Centroid}; //!< Polar, a point on the axis
        bool rotateCopies{true};         //!< Polar, false slides the copies along the arc unrotated

        bool copyLoadsAndBCs{true};
        double weldTolerance{0.0}; //!< 0 leaves the copies unwelded
    };

    /**
     * Repeats the selection, keeping the original.
     *
     * Adds geometry rather than moving it, so like mirror() it is not part of
     * the preview session. Welding happens once at the end rather than per
     * copy, and the whole command is a single undo entry.
     *
     * The original and every copy are left selected, so a second array in
     * another direction turns one bay into a grid.
     */
    static void array(FemViewWindow &view, const ArrayParams &params);

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
    // Duplicating commands - mirror() and array()
    //
    // Both collect the same set, copy it under a matrix and weld afterwards, so
    // the copy semantics live in one place rather than being reimplemented per
    // command.

    /** What a duplicating command carries across to the copy. */
    struct CopyOptions {
        bool loadsAndBCs{false};

        /**
         * Whether the transform leaves global directions alone.
         *
         * True for a translation. False for anything that rotates or reflects,
         * which makes a global load vector or a partial support mean something
         * different on the copy than it did on the original.
         */
        bool directionsPreserved{true};
    };

    /** What a duplicating command produced, accumulated over all its copies. */
    struct CopyReport {
        std::vector<ofem::Node *> nodes;
        std::vector<ofem::Beam *> beams;

        size_t skippedBCs{0};
        size_t skippedLoads{0};
    };

    /**
     * The nodes and beams a duplicating command copies.
     *
     * Selected nodes together with the ends of any selected beam, and every
     * beam with both ends in that set - which covers both gestures, selecting
     * the beams and selecting the nodes they span.
     */
    static void collectCopySet(const ofview_detail::ModelGraph &graph, std::vector<ofem::Node *> &nodes,
                               std::vector<ofem::Beam *> &beams);

    /**
     * Adds one copy of the collected set, transformed by m.
     *
     * \param checkExisting look for an existing beam on each new pair. Mirror
     *                      needs it, because a beam lying in the mirror plane
     *                      comes back onto itself; an array validates the
     *                      degenerate cases up front and can skip the scan,
     *                      which is what keeps a large array from going
     *                      quadratic.
     */
    static void duplicateOnce(FemViewWindow &view, const std::vector<ofem::Node *> &nodes,
                              const std::vector<ofem::Beam *> &beams, const glm::dmat4 &m, const CopyOptions &opts,
                              bool checkExisting, CopyReport &report);

    /** Material, cross section rotation, beam type and evaluation points. */
    static void copyBeamProps(ofem::Beam *from, ofem::Beam *to);

    /**
     * Adds the copies to the boundary conditions and loads of their sources.
     *
     * A BC or a load owns the list of objects it acts on, so a copy joins the
     * existing object rather than getting one of its own - nothing new appears
     * in the property panels, and the value stays shared, which is what "the
     * same load on every copy" means.
     */
    static void carryLoadsAndBCs(FemViewWindow &view, const std::map<ofem::Node *, ofem::Node *> &nodeCopy,
                                 const std::map<ofem::Beam *, ofem::Beam *> &beamCopy, const CopyOptions &opts,
                                 CopyReport &report);

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
