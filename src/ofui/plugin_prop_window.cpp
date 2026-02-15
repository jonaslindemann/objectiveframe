#include <ofui/plugin_prop_window.h>

#ifdef USE_FEMVIEW
#include <FemView.h>
#else
#include <FemWidget.h>
#endif

using namespace ofui;

PluginPropWindow::PluginPropWindow(const std::string name) : UiWindow(name), m_plugin{nullptr}, m_view{nullptr}
{
}

PluginPropWindow::~PluginPropWindow()
{
}

std::shared_ptr<PluginPropWindow> PluginPropWindow::create(const std::string name)
{
    return std::make_shared<PluginPropWindow>(name);
}

void ofui::PluginPropWindow::setPlugin(ScriptPlugin *plugin)
{
    m_plugin = plugin;
}

ScriptPlugin *ofui::PluginPropWindow::plugin()
{
    return m_plugin;
}

void ofui::PluginPropWindow::setView(FemViewWindow *view)
{
    m_view = view;
}

void PluginPropWindow::doDraw()
{
    ImGui::Dummy(ImVec2(150.0, 0.0));
    if (m_plugin != nullptr) {
        for (auto &name : m_plugin->paramNames()) {
            if (name != "pluginName") {
                if (m_plugin->paramType(name) == "float")
                    ImGui::InputFloat(name.c_str(), m_plugin->floatParamRef(name));
                else if (m_plugin->paramType(name) == "int")
                    ImGui::InputInt(name.c_str(), m_plugin->intParamRef(name));
                else if (m_plugin->paramType(name) == "string")
                    ImGui::InputText(name.c_str(), m_plugin->stringParamRef(name), 255);
                else
                    ImGui::TextUnformatted(name.c_str());
            }
        }
    }
    else
        ImGui::TextUnformatted("No plugin assigned");
}
