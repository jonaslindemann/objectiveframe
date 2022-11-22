#pragma once

#include <ofui/ui_window.h>

#ifdef USE_FEMVIEW
class FemViewWindow;
#else
class FemWidget;
#endif 

namespace ofui
{

class ScaleWindow : public UiWindow
{
private:
    float m_scaleFactor;
    bool m_lockScaleFactor;
    bool m_animate;
    float m_animationSpeed;


#ifdef USE_FEMVIEW
    FemViewWindow* m_view;
#else
    FemWidget* m_view;
#endif

public:
    ScaleWindow(const std::string name);
    virtual ~ScaleWindow();

#ifdef USE_FEMVIEW
    void setView(FemViewWindow* view);
#else
    void setWidget(FemWidget* femWidget);
#endif

    void update();

    static std::shared_ptr<ScaleWindow> create(const std::string name);

protected:
    virtual void doDraw();
};

typedef std::shared_ptr<ScaleWindow> ScaleWindowPtr;

}
