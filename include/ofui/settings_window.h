#pragma once

#include <ofui/ui_window.h>

class FemViewWindow;

namespace ofui {

class SettingsWindow : public UiWindow {
private:
    float m_size;
    float m_prevSize;
    float m_nodeSize;
    float m_lineRadius;
    float m_loadSize;
    float m_scaleFactor;
    float m_uiScale;
    bool m_lockScaleFactor;
    bool m_showNodeNumbers;
    bool m_offscreenRendering;
    bool m_sphereNodes;
    bool m_useImGuiFileDialogs;
    bool m_saveScreenShot;
    int m_lineSides;
    std::string m_aiApiKey;
    char m_aiApiKeyBuf[512];

    FemViewWindow *m_view;

public:
    SettingsWindow(const std::string name);
    virtual ~SettingsWindow();

    void setFemView(FemViewWindow *view);

    void update();

    static std::shared_ptr<SettingsWindow> create(const std::string name);

protected:
    virtual void doDraw();
};

typedef std::shared_ptr<SettingsWindow> SettingsWindowPtr;

} // namespace ofui
