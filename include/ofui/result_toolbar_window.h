#pragma once

#include <ofui/ui_window.h>

#include <memory>
#include <string>

class FemViewWindow;

namespace ofui {

/// Vertical toolbar for switching the displayed beam result type and toggling
/// x-ray mode. It pins itself to the right edge of the viewport, directly below
/// the window given to setAnchorWindow() (normally the coordinate display).

class ResultToolbarWindow : public UiWindow {
private:
    FemViewWindow *m_view;
    std::weak_ptr<UiWindow> m_anchorWindow;
    float m_contentWidth;

    bool resultButton(const char *label, int resultType, float width);

public:
    ResultToolbarWindow(const std::string name);
    virtual ~ResultToolbarWindow();

    static std::shared_ptr<ResultToolbarWindow> create(const std::string name);

    void setView(FemViewWindow *view);
    void setAnchorWindow(std::shared_ptr<UiWindow> window);

    void setContentWidth(float width);
    float contentWidth() const;

protected:
    virtual void doPreDraw();
    virtual void doDraw();
};

typedef std::shared_ptr<ResultToolbarWindow> ResultToolbarWindowPtr;

} // namespace ofui
