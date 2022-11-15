#pragma once

#include <ofui/ui_window.h>

#include <script_plugin.h>

#ifdef USE_FEMVIEW
class FemView;
#else
class FemWidget;
#endif

namespace ofui
{

class PluginPropWindow : public UiWindow
{
private:
    ScriptPlugin* m_plugin;
#ifdef USE_FEMVIEW
    FemView* m_view;
#else
    FemWidget* m_view;
#endif

public:
    PluginPropWindow(const std::string name);
    virtual ~PluginPropWindow();

    static std::shared_ptr<PluginPropWindow> create(const std::string name);

    void setPlugin(ScriptPlugin* plugin);
    ScriptPlugin* plugin();

#ifdef USE_FEMVIEW
    void setView(FemView* view);
#else
    void setWidget(FemWidget* widget);
#endif

protected:
    virtual void doDraw();
};

typedef std::shared_ptr<PluginPropWindow> PluginPropWindowPtr;

}
