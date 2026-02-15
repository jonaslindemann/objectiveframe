#pragma once

#include <ofui/ui_window.h>

class FemViewWindow;

namespace ofui {

class ScaleWindow : public UiWindow {
private:
    float m_scaleFactor;
    bool m_lockScaleFactor;
    bool m_animate;
    float m_animationSpeed;
    float m_maxScale;
    float m_minScale;

    FemViewWindow *m_view;

public:
    ScaleWindow(const std::string name);
    virtual ~ScaleWindow();

    void setView(FemViewWindow *view);

    void update();

    static std::shared_ptr<ScaleWindow> create(const std::string name);

protected:
    virtual void doDraw();
};

typedef std::shared_ptr<ScaleWindow> ScaleWindowPtr;

} // namespace ofui
