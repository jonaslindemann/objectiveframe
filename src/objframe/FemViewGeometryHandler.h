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
 * Every command takes a snapshot first, so all of them are undoable, and every
 * command drops the view back to the geometry representation. Moving nodes
 * while displacements are on screen would otherwise blend the new geometry
 * with the old deformation, because vfem::Node::refresh() draws a node at its
 * model coordinate plus its scaled displacement.
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

    static void translate(FemViewWindow &view, double dx, double dy, double dz);

    static void scale(FemViewWindow &view, double sx, double sy, double sz, Origin origin);

    /** Rotates by angleDeg about the axis (ax, ay, az) through origin. */
    static void rotate(FemViewWindow &view, double ax, double ay, double az, double angleDeg, Origin origin);

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
     */
    static void mirror(FemViewWindow &view, int axis, Origin origin, double weldTolerance);

    /**
     * Scales perpendicular to an axis by a factor that varies along it.
     *
     * \param axis 0 for x, 1 for y, 2 for z
     * \param s0   factor at the low end of the selection
     * \param s1   factor at the high end
     */
    static void taper(FemViewWindow &view, int axis, double s0, double s1, Origin origin);

private:
    /**
     * Collects the nodes a command should act on, honouring the pin policy.
     *
     * Returns false and reports on the console when the selection is empty or
     * everything in it is pinned.
     */
    static bool gatherNodes(FemViewWindow &view, const std::string &what, const PinPolicy &pins,
                            std::vector<ofem::Node *> &nodes);

    /** Snapshot for undo, and drop back to undeformed geometry. */
    static void begin(FemViewWindow &view);

    /**
     * Resolves an Origin against the points a command is about to transform.
     *
     * A member rather than a free function because Origin::Cursor reads the
     * last picked position out of the view.
     */
    static glm::dvec3 originFor(FemViewWindow &view, Origin origin, const std::vector<glm::dvec3> &pts);

    /** Push the moved coordinates into the scene and report. */
    static void commit(FemViewWindow &view, const std::string &what, size_t moved);

    static bool hasAnyBC(FemViewWindow &view, ofem::Node *node);
    static bool hasAnyLoad(FemViewWindow &view, ofem::Node *node);
};
