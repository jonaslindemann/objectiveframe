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
    if (m_filter.IsActive()) {
        for (int line_no = 0; line_no < m_lineOffsets.Size; line_no++) {
            const char *line_start = buf + m_lineOffsets[line_no];
            const char *line_end =
                (line_no + 1 < m_lineOffsets.Size) ? (buf + m_lineOffsets[line_no + 1] - 1) : buf_end;
            if (m_filter.PassFilter(line_start, line_end))
                ImGui::TextUnformatted(line_start, line_end);
        }
    }
    else {
        ImGuiListClipper clipper;
        clipper.Begin(m_lineOffsets.Size);
        while (clipper.Step()) {
            for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++) {
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
    ImGui::SetNextWindowSize(ImVec2(700, 40), 0); // ImGuiCond_FirstUseEver);

    const int corner = 1;
    const float PAD = 0.0f;
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
    ImVec2 work_size = viewport->WorkSize;
    ImVec2 window_pos, window_pos_pivot;
    window_pos.x = work_pos.x + work_size.x / 2 - this->width() / 2;
    window_pos.y = work_size.y - this->height() - PAD;
    window_pos_pivot.x = 0.5f;
    window_pos_pivot.y = 1.0f;
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
}
