#include <ofui/coord_window.h>

#include <algorithm>
#include <cstdio>

using namespace ofui;

CoordWindow::CoordWindow(const std::string name)
    : UiWindow(name), m_coord{0.0, 0.0, 0.0}, m_contentWidth{130.0f}, m_selectedNodes{0}, m_selectedBeams{0},
      m_selectionFilter{"All"}
{
    this->setWindowFlags(ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                         ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                         ImGuiWindowFlags_NoNav);

    this->setAutoPlacement(false);
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

void CoordWindow::setSelectionCount(int nodes, int beams)
{
    m_selectedNodes = nodes;
    m_selectedBeams = beams;
}

void CoordWindow::setSelectionFilter(const std::string filter)
{
    m_selectionFilter = filter;
}

void CoordWindow::setContentWidth(float width)
{
    m_contentWidth = width;
}

float CoordWindow::contentWidth() const
{
    return m_contentWidth;
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

void CoordWindow::drawCoord(const char *label, double value)
{
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.2f", value);

    ImGui::TextUnformatted(label);

    // Right-align the value against the content width, so the window keeps the
    // same width as the result toolbar below it regardless of the value length.

    float labelWidth = ImGui::CalcTextSize(label).x;
    float valueWidth = ImGui::CalcTextSize(buffer).x;
    float offset = m_contentWidth - valueWidth;

    ImGui::SameLine(std::max(offset, labelWidth + ImGui::GetStyle().ItemSpacing.x));
    ImGui::TextUnformatted(buffer);
}

void CoordWindow::drawValue(const char *label, const std::string &value)
{
    ImGui::TextUnformatted(label);

    float labelWidth = ImGui::CalcTextSize(label).x;
    float valueWidth = ImGui::CalcTextSize(value.c_str()).x;
    float offset = m_contentWidth - valueWidth;

    ImGui::SameLine(std::max(offset, labelWidth + ImGui::GetStyle().ItemSpacing.x));
    ImGui::TextUnformatted(value.c_str());
}

void CoordWindow::doDraw()
{
    ImGui::Dummy(ImVec2(m_contentWidth, 0.0f));
    this->drawCoord("X", m_coord[0]);
    this->drawCoord("Y", m_coord[1]);
    this->drawCoord("Z", m_coord[2]);

    // Selection readout. Always drawn, so the window keeps a constant height
    // and the toolbar anchored below it does not jump around.

    ImGui::Separator();
    this->drawValue("Filter", m_selectionFilter);
    this->drawValue("Sel. nodes", std::to_string(m_selectedNodes));
    this->drawValue("Sel. beams", std::to_string(m_selectedBeams));
}
