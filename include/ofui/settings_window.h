#pragma once

#include <ofui/ui_window.h>

#ifdef USE_FEMVIEW
class FemView;
#else
class FemWidget;
#endif

namespace ofui
{

class SettingsWindow : public UiWindow
{
private:
    float m_size;
    float m_nodeSize;
    float m_lineRadius;
    float m_loadSize;
    float m_scaleFactor;
    bool m_lockScaleFactor;
    bool m_showNodeNumbers;
    bool m_offscreenRendering;

#ifdef USE_FEMVIEW
    FemView* m_view;
#else
    FemWidget* m_view;
#endif

public:
    SettingsWindow(const std::string name);
    virtual ~SettingsWindow();

#ifdef USE_FEMVIEW
    void setFemView(FemView* view);
#else
    void setFemWidget(FemWidget* femWidget);
#endif

    void update();

    static std::shared_ptr<SettingsWindow> create(const std::string name);

protected:
    virtual void doDraw();
};

typedef std::shared_ptr<SettingsWindow> SettingsWindowPtr;

}
