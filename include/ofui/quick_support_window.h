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
 * Offers the five standard supports and nothing else. A support that is not one
 * of these is what the Node BC panel is for.
 */
class QuickSupportWindow : public UiWindow {
private:
    struct Fields {
        // Matches ofem::BeamNodeBC::DefaultKind's ordinal - 0 fixed, 1 pinned,
        // 2/3/4 roller in x/y/z. The .cpp is the only place that knows that.
        int constraint{0};
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
