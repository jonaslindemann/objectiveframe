#include <ofui/coord_window.h>

using namespace ofui;

CoordWindow::CoordWindow(const std::string name) : UiWindow(name), m_coord{0.0, 0.0, 0.0}
{
    this->setWindowFlags(ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                         ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                         ImGuiWindowFlags_NoNav);
}

CoordWindow::~CoordWindow()
{}

void CoordWindow::setCoord(double x, double y, double z)
{
    m_coord[0] = x;
    m_coord[1] = y;
    m_coord[2] = z;
}

void CoordWindow::coord(double &x, double &y, double &z)
{
    x = m_coord[0];
    y = m_coord[1];
    z = m_coord[2];
}

std::shared_ptr<CoordWindow> CoordWindow::create(const std::string name)
{
    return std::make_shared<CoordWindow>(name);
}

void CoordWindow::doPreDraw()
{
    const int corner = 1;
    const float PAD = 10.0f;
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
    ImVec2 work_size = viewport->WorkSize;
    ImVec2 window_pos, window_pos_pivot;
    window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
    window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
    window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
    window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
}

void CoordWindow::doDraw()
{
    ImGui::Dummy(ImVec2(100.0, 0.0));
    ImGui::Text("X = %10.2f", m_coord[0]);
    ImGui::Text("Y = %10.2f", m_coord[1]);
    ImGui::Text("Z = %10.2f", m_coord[2]);
}
