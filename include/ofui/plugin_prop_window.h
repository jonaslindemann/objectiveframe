#pragma once

#include <ofui/ui_window.h>

#include <script_plugin.h>

class FemWidget;

namespace ofui
{

class PluginPropWindow : public UiWindow
{
private:
    ScriptPlugin* m_plugin;
    FemWidget* m_widget;

public:
    PluginPropWindow(const std::string name);
    virtual ~PluginPropWindow();

    static std::shared_ptr<PluginPropWindow> create(const std::string name);

    void setPlugin(ScriptPlugin* plugin);
    ScriptPlugin* plugin();

    void setWidget(FemWidget* widget);

protected:
    virtual void doDraw();
};

typedef std::shared_ptr<PluginPropWindow> PluginPropWindowPtr;

}
