#pragma once

#include <string>
#include <vector>

#include <ofem/beam_node_bc.h>

namespace ofem {
class BeamNodeLoad;
class Node;
} // namespace ofem

namespace vfem {
class Node;
}

namespace ivf {
class Shape;
}

class FemViewWindow;

/**
 * Quick force and support tools.
 *
 * These exist for the beginner case: put a load or a support on some nodes
 * without first opening a dialog, naming a load, dialling in six dofs and
 * assigning it. Everything the user picks is one number and one direction, or
 * one of a handful of standard supports.
 *
 * Two surfaces drive the same commands. applyForceToSelection() and
 * applyConstraintToSelection() act on whatever is selected, which is what the
 * panel buttons call. stampShape() applies to a single node as a paint stroke
 * passes over it, which is what WidgetMode::PaintLoad and WidgetMode::PaintBC
 * call through FemViewWindow::onStamp().
 *
 * \section quickgrouping How loads and supports are grouped
 *
 * Neither tool creates a new object per click.
 *
 * A support is a pattern, not a quantity - six flags and six values - so every
 * node with the same support can share one BC. The five standard supports are
 * therefore five named, read only BCs, created on first use and found again by
 * name (ofem::BeamNodeBC::DefaultKind). A node carries at most one of them:
 * applying a support removes the node from every other node BC first, so the
 * buttons behave like the radio buttons they look like rather than
 * accumulating supports a node never visibly lost.
 *
 * A load is a quantity - one direction and one magnitude shared by all its
 * nodes - so it cannot be grouped that way. Quick loads instead coalesce: a
 * stamp joins the existing load with the same direction and magnitude, and only
 * makes a new one when nothing matches. A hundred clicks of -5 in y therefore
 * leave one load with a hundred nodes, one entry in the load list and one
 * slider in the load mixer, while a load of a different size still gets its own
 * entry. ofem::BeamNodeLoad::autoName() names whatever is created.
 *
 * \section quickundo Undo granularity
 *
 * A snapshot serialises the whole model, so one per stamped node would be both
 * slow and useless - undo would walk back a stroke one node at a time. A stroke
 * takes a single snapshot on the first node it actually changes, via the
 * stampArmed latch, exactly as a node drag does through onMoveStart().
 *
 * Every command here works out what would change before changing anything, so
 * a command with nothing to do leaves no undo entry, and the snapshot always
 * predates the first mutation - including the creation of a load or a BC, which
 * is itself a change undo has to be able to take back.
 */
class FemViewQuickToolHandler {
public:
    using ConstraintKind = ofem::BeamNodeBC::DefaultKind;

    /** What a quick force is: a direction, and how hard. */
    struct ForceSpec {
        double dx{0.0};
        double dy{-1.0};
        double dz{0.0};
        double magnitude{1.0};

        /** False for a force with no direction or no size, which cannot be applied. */
        bool valid() const;
    };

    // Commands over the current selection. Each returns how many nodes it
    // actually changed, so the caller can tell "nothing selected" from "already
    // done" rather than reporting silence either way.

    static int applyForceToSelection(FemViewWindow &view, const ForceSpec &spec);

    /**
     * Applies a force to one node, named by model index.
     *
     * The entry point for the REST API and scripting, which address nodes by
     * index rather than through the selection. Shares the coalescing and naming
     * of every other path, so a load added over HTTP lands in the same load as
     * the same force painted by hand.
     *
     * Takes no snapshot, like the other per item "...At" mutators: the caller
     * is adding loads in a loop and places its own undo points. Returns false
     * for an index out of range, a force with no size, or a node that already
     * carries the load.
     */
    static bool applyForceToNodeAt(FemViewWindow &view, int nodeIndex, const ForceSpec &spec);
    static int applyConstraintToSelection(FemViewWindow &view, ConstraintKind kind);
    static int clearForcesFromSelection(FemViewWindow &view);
    static int clearConstraintsFromSelection(FemViewWindow &view);

    // Paint stroke stamping, driven from FemViewWindow::onStamp().

    /**
     * Applies (or removes) the current quick tool on the node behind one shape.
     *
     * Takes the stroke's single snapshot the first time it changes anything,
     * and refreshes only the load or BC it touched, so the glyph appears under
     * the cursor as the stroke passes. Shapes that are not nodes are ignored.
     */
    static void stampShape(FemViewWindow &view, ivf::Shape *shape, bool remove);

    /** Arms the snapshot latch. Called when a paint stroke starts. */
    static void armStamp(FemViewWindow &view);

    /** A one line description of what the current quick tool would do. */
    static std::string forceDescription(const ForceSpec &spec);
    static std::string constraintDescription(ConstraintKind kind);

private:
    static void stampLoad(FemViewWindow &view, ofem::Node *node, bool remove);
    static void stampConstraint(FemViewWindow &view, ofem::Node *node, bool remove);

    /** The load already carrying this direction and magnitude, or null. */
    static ofem::BeamNodeLoad *findLoad(FemViewWindow &view, const ForceSpec &spec);

    /**
     * Adds a load for this direction and magnitude, named and made visible.
     *
     * Separate from findLoad() because adding a load is a model change: the
     * caller has to have snapshotted before calling this.
     */
    static ofem::BeamNodeLoad *createLoad(FemViewWindow &view, const ForceSpec &spec);

    /**
     * The BC for this standard support, or null if the model has none yet.
     *
     * The two oldest defaults are held by vfem::BeamModel, so those come from
     * it rather than from the name scan - it is the same object the older Edit
     * menu commands use.
     */
    static ofem::BeamNodeBC *findBC(FemViewWindow &view, ConstraintKind kind);

    /** Adds the BC for a standard support. Snapshot before calling, as with createLoad(). */
    static ofem::BeamNodeBC *createBC(FemViewWindow &view, ConstraintKind kind);

    static bool loadContains(ofem::BeamNodeLoad *load, ofem::Node *node);
    static bool nodeHasLoad(FemViewWindow &view, ofem::Node *node);
    static bool nodeHasBC(FemViewWindow &view, ofem::Node *node);

    static void removeNodeFromAllLoads(FemViewWindow &view, ofem::Node *node);

    /** Removes the node from every node BC, read only defaults included. */
    static void removeNodeFromAllBCs(FemViewWindow &view, ofem::Node *node);

    /** The model nodes behind the current selection, beams and everything else dropped. */
    static std::vector<ofem::Node *> selectedNodes(FemViewWindow &view);

    /** Snapshot once per gesture - see \ref quickundo. */
    static void snapShotOnce(FemViewWindow &view);

    /** Marks the model as needing a new solution and drops any stale eigenmodes. */
    static void invalidateResults(FemViewWindow &view);

    static vfem::Node *asVisualNode(ivf::Shape *shape);
};
