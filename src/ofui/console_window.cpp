#include <ofui/console_window.h>

using namespace ofui;

ConsoleWindow::ConsoleWindow(const std::string name) : UiWindow(name), m_autoScroll{false}
{
    // setWindowFlags(ImGuiWindowFlags_None);
    this->setWindowFlags(ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                         ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                         ImGuiWindowFlags_NoNav);

    clear();
}

std::shared_ptr<ConsoleWindow> ofui::ConsoleWindow::create(const std::string name)
{
    return std::make_shared<ConsoleWindow>(name);
}

void ofui::ConsoleWindow::setAnchorWindow(std::shared_ptr<UiWindow> window)
{
    m_anchorWindow = window;
}

void ofui::ConsoleWindow::realign()
{
    m_placed = false;
    m_anchorBottom = -1;
    m_stableFrames = 0;
}

void ofui::ConsoleWindow::clear()
{
    m_buffer.clear();
    m_lineOffsets.clear();
    m_lineOffsets.push_back(0);
}

void ofui::ConsoleWindow::log(const std::string message)
{
    int old_size = m_buffer.size();
    m_buffer.append(message.c_str());
    for (int new_size = m_buffer.size(); old_size < new_size; old_size++)
        if (m_buffer[old_size] == '\n')
            m_lineOffsets.push_back(old_size + 1);
}

void ofui::ConsoleWindow::doDraw()
{
    ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_NoScrollbar);

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    const char *buf = m_buffer.begin();
    const char *buf_end = m_buffer.end();
    if (m_filter.IsActive())
    {
        for (int line_no = 0; line_no < m_lineOffsets.Size; line_no++)
        {
            const char *line_start = buf + m_lineOffsets[line_no];
            const char *line_end =
                (line_no + 1 < m_lineOffsets.Size) ? (buf + m_lineOffsets[line_no + 1] - 1) : buf_end;
            if (m_filter.PassFilter(line_start, line_end))
                ImGui::TextUnformatted(line_start, line_end);
        }
    }
    else
    {
        ImGuiListClipper clipper;
        clipper.Begin(m_lineOffsets.Size);
        while (clipper.Step())
        {
            for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
            {
                const char *line_start = buf + m_lineOffsets[line_no];
                const char *line_end =
                    (line_no + 1 < m_lineOffsets.Size) ? (buf + m_lineOffsets[line_no + 1] - 1) : buf_end;
                ImGui::TextUnformatted(line_start, line_end);
            }
        }
        clipper.End();
    }
    ImGui::PopStyleVar();

    // if (m_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
    //     ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();
}

void ofui::ConsoleWindow::doPreDraw()
{
    const float scale = ImGui::GetIO().FontGlobalScale;
    const float width = 700.0f * scale;
    const float height = 40.0f * scale;

    this->setSize(int(width), int(height));
    ImGui::SetNextWindowSize(ImVec2(width, height), 0); // ImGuiCond_FirstUseEver);

    if (m_placed)
        return;

    // Placed under the modeling toolbar at startup and after each realign(), and
    // left alone in between. The toolbar is AlwaysAutoResize, so its height takes
    // a few frames to settle (and changes with the UI scale): keep aligning until
    // its bottom edge has held still for a few frames. Until the toolbar has been
    // drawn at all, park the console at the bottom centre of the work area so it
    // does not flash at ImGui's default.

    const int settleFrames = 3;
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    float x = viewport->WorkPos.x + viewport->WorkSize.x / 2.0f - width / 2.0f;

    auto anchor = m_anchorWindow.lock();
    const bool noAnchor = anchor == nullptr || !anchor->visible();
    if (noAnchor || anchor->y() < 0 || anchor->height() <= 0)
    {
        const float pad = 20.0f * scale;
        ImGui::SetNextWindowPos(ImVec2(x, viewport->WorkPos.y + viewport->WorkSize.y - height - pad),
                                ImGuiCond_Always);

        // Nothing to align with at all: the fallback is the placement.

        if (noAnchor)
            m_placed = true;
        return;
    }

    const int anchorBottom = anchor->y() + anchor->height();
    if (anchorBottom == m_anchorBottom)
        m_stableFrames++;
    else
    {
        m_anchorBottom = anchorBottom;
        m_stableFrames = 0;
    }

    // Centred when that clears the toolbar, otherwise just to its right. Both
    // share a bottom edge, so centring in a narrow window would put the console
    // on top of the toolbar.

    const float margin = 10.0f * scale;
    const float minX = float(anchor->x() + anchor->width()) + margin;
    if (x < minX)
        x = minX;

    ImGui::SetNextWindowPos(ImVec2(x, float(anchorBottom) - height), ImGuiCond_Always);

    if (m_stableFrames >= settleFrames)
        m_placed = true;
}
