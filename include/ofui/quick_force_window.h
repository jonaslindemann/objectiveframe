#pragma once

#include <ofui/ui_window.h>

#include <cmath>

class FemViewWindow;

namespace ofui {

/**
 * The quick force tool.
 *
 * Says what WidgetMode::PaintLoad will apply, and applies the same thing to the
 * current selection on demand - so a beginner can either drag over nodes or
 * select them first, and get the same result either way.
 *
 * Deliberately small: one magnitude and one of six axis directions. Anything
 * beyond that is what the Node loads panel is for, and this panel says which
 * load the next click will feed so the two can be followed between.
 */
class QuickForceWindow : public UiWindow {
private:
    /**
     * The widget values, kept as plain scalars.
     *
     * ofui must not include objframe headers, so the panel cannot hold a
     * FemViewQuickToolHandler::ForceSpec. The .cpp is the only place that knows
     * how these map onto one.
     */
    struct Fields {
        // Double, not float: the magnitude ends up in the load's name, and a
        // float 0.1 widened to double prints as 0.1000000015.
        double magnitude{1.0};

        // 0 -x, 1 +x, 2 -y, 3 +y, 4 -z, 5 +z. Down is the default because it is
        // what a beginner reaching for "add a force" almost always means.
        int direction{2};
    };
    Fields m_fields;

    FemViewWindow *m_view;

    void pushForce();

public:
    QuickForceWindow(const std::string name);
    virtual ~QuickForceWindow();

    static std::shared_ptr<QuickForceWindow> create(const std::string name);

    void setFemView(FemViewWindow *view);

    /** Re-reads the view, so the panel agrees with whatever a script last set. */
    void update();

protected:
    virtual void doDraw();
};

typedef std::shared_ptr<QuickForceWindow> QuickForceWindowPtr;

} // namespace ofui
