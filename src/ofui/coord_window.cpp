#include <ofui/coord_window.h>

#include <algorithm>
#include <cstdio>

using namespace ofui;

CoordWindow::CoordWindow(const std::string name)
    : UiWindow(name), m_coord{0.0, 0.0, 0.0}, m_contentWidth{130.0f}, m_selectedNodes{0}, m_selectedBeams{0},
      m_selectionFilter{"All"}, m_rowWidth{130.0f}
{
    this->setWindowFlags(ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                         ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                         ImGuiWindowFlags_NoNav);

    this->setAutoPlacement(false);

    // A readout, not a panel: clicking its release button must not count as
    // focusing a window, or the view would stop taking keyboard shortcuts for
    // as long as the click left it focused. The toolbars opt out for the same
    // reason.

    this->setIgnoreFocusCheck(true);
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

void CoordWindow::setWorkPlane(const WorkPlaneInfo &info)
{
    m_workPlane = info;
}

void CoordWindow::assignPlaneClickedFunc(const PlaneClickedFunc &func)
{
    m_onPlaneClicked = func;
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
    const float PAD = 10.0f * ImGui::GetIO().FontGlobalScale;
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
    float offset = m_rowWidth - valueWidth;

    ImGui::SameLine(std::max(offset, labelWidth + ImGui::GetStyle().ItemSpacing.x));
    ImGui::TextUnformatted(buffer);
}

void CoordWindow::drawValue(const char *label, const std::string &value)
{
    ImGui::TextUnformatted(label);

    float labelWidth = ImGui::CalcTextSize(label).x;
    float valueWidth = ImGui::CalcTextSize(value.c_str()).x;
    float offset = m_rowWidth - valueWidth;

    ImGui::SameLine(std::max(offset, labelWidth + ImGui::GetStyle().ItemSpacing.x));
    ImGui::TextUnformatted(value.c_str());
}

void CoordWindow::drawPlaneButton(const char *label, int plane)
{
    // Disabled means "you are already here". For xz that is the default ground
    // plane, which is the unlocked state rather than the xz plane as such - a
    // horizontal plane locked at some height is a different thing, and clicking
    // xz is how you get back out of it.

    bool current = (plane == 0) ? !m_workPlane.locked : (m_workPlane.locked && plane == m_workPlane.plane);
    bool pending = (plane == m_workPlane.pending);

    if (pending)
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.98f, 0.60f, 0.15f, 0.40f));

    ImGui::BeginDisabled(current || !m_workPlane.enabled);

    if (ImGui::SmallButton(label) && m_onPlaneClicked)
        m_onPlaneClicked(plane);

    ImGui::EndDisabled();

    if (pending)
        ImGui::PopStyleColor();
}

void CoordWindow::doDraw()
{
    // The window auto-resizes to its widest row, which is not necessarily a
    // coordinate row - the plane buttons can be wider. Measured here, with the
    // cursor still at the left edge, so every row below right-aligns against
    // the same edge the window actually ends at.

    m_rowWidth = (std::max)(m_contentWidth * ImGui::GetIO().FontGlobalScale, ImGui::GetContentRegionAvail().x);

    ImGui::Dummy(ImVec2(m_contentWidth * ImGui::GetIO().FontGlobalScale, 0.0f));
    this->drawCoord("X", m_coord[0]);
    this->drawCoord("Y", m_coord[1]);
    this->drawCoord("Z", m_coord[2]);

    // Which plane a click will land on. Worth having on screen permanently:
    // without it the only clue is whether [Shift] is down, which is invisible,
    // and a locked plane has no clue at all.

    ImGui::Separator();

    ImGui::TextUnformatted("Plane");
    ImGui::SameLine();
    this->drawPlaneButton("XZ", 0);
    ImGui::SameLine();
    this->drawPlaneButton("XY", 1);
    ImGui::SameLine();
    this->drawPlaneButton("YZ", 2);

    // Always a line here, blank when there is nothing to say, so the window
    // keeps a constant height and the toolbar anchored below it does not jump
    // whenever the plane is locked or freed.

    ImGui::TextDisabled("%s", m_workPlane.locked ? m_workPlane.text.c_str() : " ");

    // Selection readout. Always drawn, so the window keeps a constant height
    // and the toolbar anchored below it does not jump around.

    ImGui::Separator();
    this->drawValue("Filter", m_selectionFilter);
    this->drawValue("Sel. nodes", std::to_string(m_selectedNodes));
    this->drawValue("Sel. beams", std::to_string(m_selectedBeams));
}
