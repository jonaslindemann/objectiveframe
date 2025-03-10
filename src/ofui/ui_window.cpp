#include <ofui/ui_window.h>

#include <imgui.h>

using namespace ofui;

UiWindow::UiWindow(const std::string name)
    : m_name{name}, m_visible{true}, m_windowFlags{ImGuiWindowFlags_AlwaysAutoResize}, m_updatePos{false},
      m_centerBottom{false}, m_corner{-1}, m_setPos{false}, m_center{false},
      m_firstDraw{true}, m_width{-1}, m_height{-1}, m_canClose{true}, m_x{-1}, m_y{-1}, m_newPos{false},
      m_autoPlacement{true}, m_newBottomPos{false}, m_isFocused{false}, m_ignoreFocusCheck{false}
{}

UiWindow::~UiWindow()
{}

std::shared_ptr<UiWindow> UiWindow::create(const std::string name)
{
    return std::make_shared<UiWindow>(name);
}

void UiWindow::draw()
{
    if (m_visible)
    {
        if ((m_width != -1) && (m_height != -1))
            ImGui::SetNextWindowSize(ImVec2(float(m_width), float(m_height)), ImGuiCond_FirstUseEver);

        if (m_setPos)
        {
            const float PAD = 80.0f;
            const ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
            ImVec2 work_size = viewport->WorkSize;
            ImVec2 window_pos, window_pos_pivot;
            window_pos.x = (m_corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
            window_pos.y = (m_corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
            window_pos_pivot.x = (m_corner & 1) ? 1.0f : 0.0f;
            window_pos_pivot.y = (m_corner & 2) ? 1.0f : 0.0f;
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
            m_setPos = false;
        }
        if (m_newPos)
        {
            auto viewport = ImGui::GetMainViewport();
            ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
            ImVec2 work_size = viewport->WorkSize;
            ImGui::SetNextWindowPos(ImVec2(work_pos.x + float(m_x), work_pos.y + float(m_y)), ImGuiCond_Always);
            m_newPos = false;
        }
        if (m_newBottomPos)
        {
            auto viewport = ImGui::GetMainViewport();
            ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
            ImVec2 work_size = viewport->WorkSize;
            m_y = work_size.y - m_y;
            ImGui::SetNextWindowPos(ImVec2(work_pos.x + float(m_x), work_pos.y + float(m_y)), ImGuiCond_Always);
            m_newBottomPos = false;
        }
        if (m_updatePos)
        {
            const float PAD_X = 100.0f;
            const float PAD_Y = 150.0f;
            const ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
            ImVec2 work_size = viewport->WorkSize;
            ImVec2 window_pos, window_pos_pivot;
            window_pos.x = (m_corner & 1) ? (work_pos.x + work_size.x - PAD_X) : (work_pos.x + PAD_X);
            window_pos.y = (m_corner & 2) ? (work_pos.y + work_size.y - PAD_Y) : (work_pos.y + PAD_Y);
            window_pos_pivot.x = (m_corner & 1) ? 1.0f : 0.0f;
            window_pos_pivot.y = (m_corner & 2) ? 1.0f : 0.0f;
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Appearing, window_pos_pivot);
            m_updatePos = false;
        }
        if (m_centerBottom)
        {
            const float PAD = 50.0f;
            const ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
            ImVec2 work_size = viewport->WorkSize;
            ImVec2 window_pos, window_pos_pivot;
            window_pos.x = work_size.x / 2.0f;
            window_pos.y = work_size.y - PAD;
            window_pos_pivot.x = 0.5;
            window_pos_pivot.y = 1.0;
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Appearing, window_pos_pivot);
            m_centerBottom = false;
        }
        if (m_center)
        {
            const ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
            ImVec2 work_size = viewport->WorkSize;
            ImVec2 window_pos, window_pos_pivot;

            ImVec2 size = ImGui::GetWindowSize();
            m_width = int(size.x);
            m_height = int(size.y);

            window_pos.x = work_size.x / 2.0f - m_width / 2.0;
            window_pos.y = work_size.y / 2.0f - m_height / 2.0;
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always);
            m_center = false;
        }
        doPreDraw();
        if (m_canClose)
        {
            ImGui::Begin(m_name.c_str(), &m_visible, m_windowFlags); //, nullptr, ImGuiWindowFlags_AlwaysAutoResize);
            m_isFocused = ImGui::IsWindowFocused();
        }
        else
        {
            ImGui::Begin(m_name.c_str(), nullptr, m_windowFlags);
            m_isFocused = ImGui::IsWindowFocused();
        }
        doDraw();

        if (!m_firstDraw)
        {
            ImVec2 size = ImGui::GetWindowSize();
            m_width = int(size.x);
            m_height = int(size.y);

            ImVec2 pos = ImGui::GetWindowPos();
            m_x = int(pos.x);
            m_y = int(pos.y);
        }

        ImGui::End();
        doPostDraw();
    }
    m_firstDraw = false;
}

void UiWindow::setWindowFlags(ImGuiWindowFlags flags)
{
    m_windowFlags = flags;
}

ImGuiWindowFlags UiWindow::windowFlags()
{
    return m_windowFlags;
}

void UiWindow::setVisible(bool flag)
{
    m_visible = flag;
}

bool UiWindow::visible()
{
    return m_visible;
}

void UiWindow::show()
{
    m_visible = true;
}

void UiWindow::hide()
{
    m_visible = false;
}

void UiWindow::setSize(int w, int h)
{
    ImGui::SetWindowSize(m_name.c_str(), ImVec2(float(w), float(h)));
    m_width = w;
    m_height = h;
}

int ofui::UiWindow::width()
{
    return m_width;
}

int ofui::UiWindow::height()
{
    return m_height;
}

bool ofui::UiWindow::autoPlacement()
{
    return m_autoPlacement;
}

void ofui::UiWindow::setAutoPlacement(bool flag)
{
    m_autoPlacement = flag;
}

void ofui::UiWindow::setIgnoreFocusCheck(bool flag)
{
    m_ignoreFocusCheck = flag;
}

std::string ofui::UiWindow::name()
{
    return m_name;
}

bool ofui::UiWindow::isFocused()
{
    if (m_ignoreFocusCheck)
        return false;

    return m_isFocused;
}

bool ofui::UiWindow::canClose()
{
    return m_canClose;
}

void ofui::UiWindow::setCanClose(bool flag)
{
    m_canClose = flag;
}

void UiWindow::align(int corner)
{
    m_corner = corner;
    m_updatePos = true;
}

void ofui::UiWindow::centerBottom()
{
    m_centerBottom = true;
}

void ofui::UiWindow::center()
{
    m_center = true;
}

void ofui::UiWindow::setPosition(int x, int y)
{
    m_x = x;
    m_y = y;
    m_newPos = true;
}

void ofui::UiWindow::setPositionFromBottom(int x, int y)
{
    m_x = x;
    m_y = y;
    m_newBottomPos = true;
}

void ofui::UiWindow::enableMenuBar()
{
    m_windowFlags |= ImGuiWindowFlags_MenuBar;
}

int ofui::UiWindow::x()
{
    return m_x;
}

int ofui::UiWindow::y()
{
    return m_y;
}

void UiWindow::doPreDraw()
{}

void UiWindow::doDraw()
{}

void UiWindow::doPostDraw()
{}
