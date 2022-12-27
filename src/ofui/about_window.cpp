#include <ofui/about_window.h>

#include <imgui.h>

using namespace ofui;

AboutWindow::AboutWindow(const std::string name)
    : UiWindow(name)
{
}

AboutWindow::~AboutWindow()
{
}

std::shared_ptr<AboutWindow> AboutWindow::create(const std::string name)
{
    return std::make_shared<AboutWindow>(name);
}

void ofui::AboutWindow::setVersion(std::string text)
{
    m_versionString = text;
}

void ofui::AboutWindow::setRelease(std::string text)
{
    m_release = text;
}

void ofui::AboutWindow::setCopyright(std::string text)
{
    m_copyright = text;
}

void ofui::AboutWindow::setAuthor1(std::string text)
{
    m_author1 = text;
}

void ofui::AboutWindow::setAuthor2(std::string text)
{
    m_author2 = text;
}

void AboutWindow::doDraw()
{
    ImGui::Dummy(ImVec2(400.0, 0.0));
    ImGui::Text(m_versionString.c_str());
    ImGui::Text(m_release.c_str());
    ImGui::Text(m_copyright.c_str());
    ImGui::Text(m_author1.c_str());
    ImGui::Text(m_author2.c_str());
}
