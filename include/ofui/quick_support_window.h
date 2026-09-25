#pragma once

#include <ofui/ui_window.h>

class FemViewWindow;

namespace ofui {

/**
 * The quick support tool.
 *
 * Says what WidgetMode::PaintBC will apply, and applies the same thing to the
 * current selection on demand - the support counterpart of
 * ofui::QuickForceWindow, and deliberately its twin in layout.
 *
 * Offers the five standard supports and nothing else, as one row of segments. A
 * support that is not one of these is what the Node BC panel is for.
 *
 * What each support holds, and that a node carries one at a time, are tooltips
 * rather than standing text - see ofui::QuickForceWindow, whose layout this
 * matches row for row.
 */
class QuickSupportWindow : public UiWindow {
private:
    struct Fields {
        // Matches ofem::BeamNodeBC::DefaultKind's ordinal - 0 fixed, 1 pinned,
        // 2/3/4 roller in x/y/z. The .cpp is the only place that knows that.
        //
        // Pinned rather than fixed: it is the support a frame usually wants, and
        // it is the weaker of the two - a model that should have been built in
        // shows up as a structure that is too soft, where full fixity where a
        // pin was meant quietly stiffens the frame and reads as correct.
        //
        // Kept in step with FemViewWindow::QuickToolState::constraint, which
        // carries the same default on the view side.
        int constraint{1};
    };
    Fields m_fields;

    FemViewWindow *m_view;

    void pushConstraint();

public:
    QuickSupportWindow(const std::string name);
    virtual ~QuickSupportWindow();

    static std::shared_ptr<QuickSupportWindow> create(const std::string name);

    void setFemView(FemViewWindow *view);

    /** Re-reads the view, so the panel agrees with whatever a script last set. */
    void update();

protected:
    virtual void doDraw();
};

typedef std::shared_ptr<QuickSupportWindow> QuickSupportWindowPtr;

} // namespace ofui
