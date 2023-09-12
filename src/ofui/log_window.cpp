#include <ofui/log_window.h>

using namespace ofui;

LogWindow::LogWindow(const std::string name) : UiWindow(name), m_autoScroll{true}
{
    setWindowFlags(ImGuiWindowFlags_None);
    clear();
}

std::shared_ptr<LogWindow> ofui::LogWindow::create(const std::string name)
{
    return std::make_shared<LogWindow>(name);
}

void ofui::LogWindow::clear()
{
    m_buffer.clear();
    m_lineOffsets.clear();
    m_lineOffsets.push_back(0);
}

void ofui::LogWindow::log(const std::string message)
{
    int old_size = m_buffer.size();
    m_buffer.append(message.c_str());
    for (int new_size = m_buffer.size(); old_size < new_size; old_size++)
        if (m_buffer[old_size] == '\n')
            m_lineOffsets.push_back(old_size + 1);
}

void ofui::LogWindow::doDraw()
{
    // Options menu
    if (ImGui::BeginPopup("Options")) {
        ImGui::Checkbox("Auto-scroll", &m_autoScroll);
        ImGui::EndPopup();
    }

    // Main window
    if (ImGui::Button("Options"))
        ImGui::OpenPopup("Options");
    ImGui::SameLine();
    bool clear = ImGui::Button("Clear");
    ImGui::SameLine();
    bool copy = ImGui::Button("Copy");
    ImGui::SameLine();
    m_filter.Draw("Filter", -100.0f);

    ImGui::Separator();
    ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    if (clear)
        this->clear();
    if (copy)
        ImGui::LogToClipboard();

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

    if (m_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();
}

void ofui::LogWindow::doPreDraw()
{
}
