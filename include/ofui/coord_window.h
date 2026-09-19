#pragma once

#include <ofui/ui_window.h>

#include <functional>

namespace ofui {

/**
 * Called when the user picks a work plane from the readout.
 *
 * The argument is the ShiftPlane ordinal - 0 xz, 1 xy, 2 yz. Picking xz means
 * "back to the ordinary ground plane", which is why there is no separate
 * release button.
 */
typedef std::function<void(int)> PlaneClickedFunc;

/** Everything the readout needs to say about the work plane. */
struct WorkPlaneInfo {
    /** The ShiftPlane ordinal - 0 xz, 1 xy, 2 yz. */
    int plane{0};

    /** Pinned there, rather than following [Shift]. */
    bool locked{false};

    /** The plane a lock is being placed for, or -1. */
    int pending{-1};

    /**
     * Whether choosing a plane would do anything at all.
     *
     * False in the modes that place no 3D cursor - selection, box select and
     * the rest. There is nothing for a work plane to constrain there, so the
     * buttons go dead rather than silently locking something the user cannot
     * see the effect of.
     */
    bool enabled{true};

    /** Where the locked plane sits, e.g. "locked at y = 2.50". */
    std::string text;
};

class CoordWindow : public UiWindow {
private:
    std::string m_name;
    double m_coord[3];
    float m_contentWidth;

    int m_selectedNodes;
    int m_selectedBeams;
    std::string m_selectionFilter;

    WorkPlaneInfo m_workPlane;

    // The width rows align their values against, measured from the window's own
    // content region each frame so it follows whatever the widest row needs -
    // the plane buttons can be wider than the coordinate rows.
    float m_rowWidth;

    PlaneClickedFunc m_onPlaneClicked;

    void drawCoord(const char *label, double value);
    void drawValue(const char *label, const std::string &value);

    /**
     * One plane button.
     *
     * Disabled when it is the state the view is already in - the default ground
     * plane for xz, a lock on that plane for xy and yz - so the enabled buttons
     * are exactly the things that would change something.
     */
    void drawPlaneButton(const char *label, int plane);

public:
    CoordWindow(const std::string name);
    virtual ~CoordWindow();

    void setCoord(double x, double y, double z);
    void coord(double &x, double &y, double &z);

    /** Number of selected objects shown under the coordinate readout. */
    void setSelectionCount(int nodes, int beams);

    /** Name of the active selection filter, e.g. "All", "Nodes", "Beams". */
    void setSelectionFilter(const std::string filter);

    /**
     * The plane cursor placement is working against.
     */
    void setWorkPlane(const WorkPlaneInfo &info);

    void assignPlaneClickedFunc(const PlaneClickedFunc &func);

    void setContentWidth(float width);
    float contentWidth() const;

    static std::shared_ptr<CoordWindow> create(const std::string name);

protected:
    virtual void doPreDraw();
    virtual void doDraw();
};

typedef std::shared_ptr<CoordWindow> CoordWindowPtr;

} // namespace ofui
