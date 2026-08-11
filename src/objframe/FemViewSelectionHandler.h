#pragma once

#include <string>

class FemViewWindow;

/**
 * Structure aware selection commands.
 *
 * These work on the beam model graph rather than on screen geometry, so they
 * can answer questions the pointer cannot - "the rest of this member", "the
 * whole connected frame", "everything at this level".
 *
 * Every command replaces the selection with its result and honours the active
 * selection filter.
 */
class FemViewSelectionHandler {
public:
    /** Expands the selection by one ring of neighbouring nodes and beams. */
    static void growSelection(FemViewWindow &view);

    /** Erodes the selection by one ring, the inverse of growSelection(). */
    static void shrinkSelection(FemViewWindow &view);

    /** Selects everything reachable through beams from the current selection. */
    static void selectConnected(FemViewWindow &view);

    /**
     * Extends selected beams along their member.
     *
     * Walks outwards through nodes that join exactly two beams as long as the
     * beams stay collinear, so a subdivided column is picked up in one go.
     */
    static void selectMember(FemViewWindow &view);

    /**
     * Selects everything lying in the same plane as the current selection.
     *
     * \param axis 0 for x, 1 for y (a storey), 2 for z
     */
    static void selectSamePlane(FemViewWindow &view, int axis);

    /** Selects every beam sharing a material with the selected beams. */
    static void selectSameMaterial(FemViewWindow &view);

private:
    /**
     * Reports the resulting selection on the console.
     *
     * Counts are read back from the view, not from the computed set, because
     * the active selection filter may have dropped part of the result.
     */
    static void report(FemViewWindow &view, const std::string what);
};
