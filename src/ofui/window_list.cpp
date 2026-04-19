#include <ofui/window_list.h>

#include <imgui.h>
#include <imgui_internal.h>

#include <algorithm>

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
    ImGuiContext *ctx = ImGui::GetCurrentContext();
    if (!ctx)
        return;

    const ImGuiViewport *vp = ImGui::GetMainViewport();
    float vpX = vp->WorkPos.x;
    float vpY = vp->WorkPos.y;
    float vpW = vp->WorkSize.x;
    float vpH = vp->WorkSize.y;

    float newW = (window->width() > 0) ? float(window->width()) : 320.0f;
    float newH = (window->height() > 0) ? float(window->height()) : 220.0f;

    // All coordinates below are work-area-relative (origin = WorkPos).
    const float START_X = 80.0f;
    const float START_Y = 60.0f;
    const float END_X   = vpW - 8.0f;
    const float END_Y   = vpH - 20.0f;
    const float PAD     = 8.0f;
    const float STEP    = 24.0f;

    // Build occupied rects using UiWindow's own tracked position/size.
    // After the first draw, w->x() / w->y() return absolute screen coordinates
    // (from ImGui::GetWindowPos). Converting to work-area-relative by subtracting
    // WorkPos avoids all FindWindowByName / WasActive timing issues.
    std::vector<ImRect> occupied;
    for (auto &w : m_windows)
    {
        if (w.get() == window.get() || !w->visible())
            continue;
        int wx = w->x(), wy = w->y(), ww = w->width(), wh = w->height();
        if (wx < 0 || wy < 0 || ww <= 0 || wh <= 0)
            continue;   // window has never been drawn — no position yet

        float relX = float(wx) - vpX;
        float relY = float(wy) - vpY;

        // Skip windows outside the main viewport (e.g. on a secondary monitor).
        if (relX < 0.0f || relX >= vpW || relY < 0.0f || relY >= vpH)
            continue;

        occupied.push_back(ImRect(relX, relY, relX + float(ww), relY + float(wh)));
    }

    float px = START_X;
    float py = START_Y;

    // Preferred: stack directly below the last window we placed.
    // m_lastX/Y/H are stored when we call setPosition, so they stay consistent
    // with the work-area-relative coordinate system used here.
    auto last = m_lastPlaced.lock();
    if (last && last->visible())
    {
        float belowY = m_lastY + m_lastH + PAD;
        if (belowY + newH <= END_Y)
        {
            px = m_lastX;
            py = belowY;
            m_lastPlaced = window;
            m_lastX = px;
            m_lastY = py;
            m_lastH = newH;
            window->setPosition(int(px), int(py));
            return;
        }
        // No room below — fall through to grid scan.
    }

    // Fallback: grid scan (left-to-right, top-to-bottom) for the first free slot.
    bool placed = false;
    for (float cy = START_Y; cy + newH <= END_Y && !placed; cy += STEP)
    {
        for (float cx = START_X; cx + newW <= END_X && !placed; cx += STEP)
        {
            ImRect candidate(cx, cy, cx + newW, cy + newH);
            bool overlaps = std::any_of(occupied.begin(), occupied.end(),
                                        [&](const ImRect &r) { return candidate.Overlaps(r); });
            if (!overlaps)
            {
                px = cx;
                py = cy;
                placed = true;
            }
        }
    }

    m_lastPlaced = window;
    m_lastX = px;
    m_lastY = py;
    m_lastH = newH;
    window->setPosition(int(px), int(py));
}

void ofui::WindowList::add(UiWindowPtr window)
{
    m_windows.push_back(window);
}

void ofui::WindowList::clear()
{
    m_windows.clear();
    m_lastPlaced.reset();
    m_lastX = 80.0f;
    m_lastY = 60.0f;
    m_lastH = 220.0f;
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

bool ofui::WindowList::isAnyFocused()
{
    for (auto &window : m_windows)
    {
        if (window->isFocused())
        {
            std::cout << "Window " << window->name() << " is focused " << std::endl;
            return true;
        }
    }
    return false;
}

void WindowList::draw()
{
    for (auto &window : m_windows)
        window->draw();
}
