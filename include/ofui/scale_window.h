#pragma once

#include <ofui/ui_window.h>

class FemWidget;

namespace ofui
{

class ScaleWindow : public UiWindow
{
private:
    float m_scaleFactor;
    bool m_lockScaleFactor;
    bool m_animate;
    float m_animationSpeed;


    FemWidget* m_widget;

public:
    ScaleWindow(const std::string name);
    virtual ~ScaleWindow();

    void setWidget(FemWidget* femWidget);

    void update();

    static std::shared_ptr<ScaleWindow> create(const std::string name);

protected:
    virtual void doDraw();
};

typedef std::shared_ptr<ScaleWindow> ScaleWindowPtr;

}
