#include <ofui/window_list.h>

#include <imgui.h>

#include <FemView.h>

using namespace ofui;

WindowList::WindowList()
{}

WindowList::~WindowList()
{}

std::shared_ptr<WindowList> ofui::WindowList::create()
{
    return std::make_shared<WindowList>();
}

void ofui::WindowList::setFemView(FemViewWindow *view)
{
    m_view = view;
}

void ofui::WindowList::findNextPosition(int &x, int &y)
{
    auto nextY = 0;

    for (auto &window : m_windows)
    {
        if (window->visible() && window->autoPlacement())
        {
            auto h = window->height();

            nextY += h + 10;
        }
    }

    y = nextY;
}

void ofui::WindowList::placeWindow(UiWindowPtr window)
{
    /*
    auto x = window->x();
    auto y = window->y();
    findNextPosition(x, y);
    window->setPosition(x, y);
    */
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
