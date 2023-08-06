#include <ofui/window_list.h>

#include <imgui.h>

using namespace ofui;

WindowList::WindowList()
{
}

WindowList::~WindowList()
{
}

std::shared_ptr<WindowList> ofui::WindowList::create()
{
    return std::make_shared<WindowList>();
}

void ofui::WindowList::add(UiWindowPtr window)
{
    m_windows.push_back(window);
}

void ofui::WindowList::clear()
{
    m_windows.clear();
}

void ofui::WindowList::hideAll()
{
    for (auto &window : m_windows)
        window->hide();
}

void ofui::WindowList::showAll()
{
    for (auto &window : m_windows)
        window->show();
}

void WindowList::draw()
{
    for (auto &window : m_windows)
        window->draw();
}
