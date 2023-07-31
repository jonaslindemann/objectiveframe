#include <ofui/popup_window.h>

#include <imgui.h>

using namespace ofui;

PopupWindow::PopupWindow(const std::string name, bool modal)
    : m_name{name}, m_visible{false}, m_windowFlags{0},
      m_updatePos{false}, m_corner{-1}, m_modal{modal}, m_closed{false}, m_modalResult{PopupResult::NONE}
{
}

PopupWindow::~PopupWindow()
{
}

std::shared_ptr<PopupWindow> PopupWindow::create(const std::string name, bool modal)
{
    return std::make_shared<PopupWindow>(name, modal);
}

void PopupWindow::draw()
{
    if (m_visible) {
        if (m_updatePos) {
            const float PAD = 10.0f;
            const ImGuiViewport *viewport = ImGui::GetMainViewport();
            ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
            ImVec2 work_size = viewport->WorkSize;
            ImVec2 window_pos, window_pos_pivot;
            window_pos.x = (m_corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
            window_pos.y = (m_corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
            window_pos_pivot.x = (m_corner & 1) ? 1.0f : 0.0f;
            window_pos_pivot.y = (m_corner & 2) ? 1.0f : 0.0f;
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
            m_updatePos = false;
        }
        doPreDraw();
        doDraw();
        doPostDraw();
    }
}

void PopupWindow::setWindowFlags(ImGuiWindowFlags flags)
{
    m_windowFlags = flags;
}

ImGuiWindowFlags PopupWindow::windowFlags()
{
    return m_windowFlags;
}

void PopupWindow::setName(std::string name)
{
    m_name = name;
}

std::string PopupWindow::name()
{
    return m_name;
}

void PopupWindow::setVisible(bool flag)
{
    m_visible = flag;
}

bool PopupWindow::visible()
{
    return m_visible;
}

void PopupWindow::show()
{
    m_modalResult = PopupResult::NONE;
    m_visible = true;
    m_closed = false;
}

void PopupWindow::hide()
{
    m_visible = false;
}

void PopupWindow::setSize(int w, int h)
{
    ImGui::SetWindowSize(m_name.c_str(), ImVec2(float(w), float(h)));
}

void PopupWindow::align(int corner)
{
    m_corner = corner;
    m_updatePos = true;
}

bool PopupWindow::closed()
{
    bool prevState = m_closed;
    m_closed = false;
    return prevState;
}

void PopupWindow::close(PopupResult modalResult)
{
    m_modalResult = modalResult;
    m_visible = false;
    m_closed = true;
}

PopupResult PopupWindow::modalResult()
{
    return m_modalResult;
}

void PopupWindow::doPreDraw()
{
}

void PopupWindow::doDraw()
{
    ImGui::OpenPopup(m_name.c_str());
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal(m_name.c_str(), NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        this->doPopup();
        ImGui::EndPopup();
    }
}

void PopupWindow::doPostDraw()
{
}

void PopupWindow::doPopup()
{
}
