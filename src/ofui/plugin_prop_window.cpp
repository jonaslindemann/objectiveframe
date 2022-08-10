#include <ofui/plugin_prop_window.h>

#include <FemWidget.h>

using namespace ofui;

PluginPropWindow::PluginPropWindow(const std::string name)
    : UiWindow(name)
    , m_plugin { nullptr }
    , m_widget { nullptr }
{
}

PluginPropWindow::~PluginPropWindow()
{
}

std::shared_ptr<PluginPropWindow> PluginPropWindow::create(const std::string name)
{
    return std::make_shared<PluginPropWindow>(name);
}

void ofui::PluginPropWindow::setPlugin(ScriptPlugin* plugin)
{
    m_plugin = plugin;
}

ScriptPlugin* ofui::PluginPropWindow::plugin()
{
    return m_plugin;
}

void ofui::PluginPropWindow::setWidget(FemWidget* widget)
{
    m_widget = widget;
}

void PluginPropWindow::doDraw()
{
    ImGui::Dummy(ImVec2(150.0, 0.0));
    if (m_plugin != nullptr)
    {
        for (auto& name : m_plugin->paramNames())
        {
            if (name != "pluginName")
            {
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
        if (m_widget!=nullptr)
        {
            if (ImGui::Button("Create"))
            {
                m_widget->runPlugin(m_plugin);
            }
        }
    }
    else
        ImGui::TextUnformatted("No plugin assigned");
}
