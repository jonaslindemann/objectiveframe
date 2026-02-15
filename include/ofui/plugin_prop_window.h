#pragma once

#include <ofui/ui_window.h>

#include <script_plugin.h>

class FemViewWindow;

namespace ofui {

class PluginPropWindow : public UiWindow {
private:
    ScriptPlugin *m_plugin;
    FemViewWindow *m_view;

public:
    PluginPropWindow(const std::string name);
    virtual ~PluginPropWindow();

    static std::shared_ptr<PluginPropWindow> create(const std::string name);

    void setPlugin(ScriptPlugin *plugin);
    ScriptPlugin *plugin();

    void setView(FemViewWindow *view);

protected:
    virtual void doDraw();
};

typedef std::shared_ptr<PluginPropWindow> PluginPropWindowPtr;

} // namespace ofui
