#include <ofui/script_window.h>

#include <ofutil/util_functions.h>

#include <FemView.h>

#include <cstring> // For strncpy
#include <fstream>

#include <imguifd/ImGuiFileDialog.h>
#include <imguifd/ImGuiFileDialogConfig.h>

using namespace ofui;

ScriptWindow::ScriptWindow(const std::string name) : UiWindow(name), m_view(nullptr), m_saveDialog(false)
{
    setWindowFlags(ImGuiWindowFlags_None);
    enableMenuBar();
    setAutoPlacement(false);

    // Initialize the buffer and content
    m_content = "";                        // Start with empty content
    memset(m_inputBuffer, 0, BUFFER_SIZE); // Clear the input buffer

    this->setSize(800, 600);
}

std::shared_ptr<ScriptWindow> ofui::ScriptWindow::create(const std::string name)
{
    return std::make_shared<ScriptWindow>(name);
}

void ofui::ScriptWindow::newScript()
{
    m_filename = "";
    m_content = "";
    m_modified = false;
    m_undoBuffer.clear();
    m_undoIndex = -1;
}

void ofui::ScriptWindow::open(const std::string &filename)
{
    m_filename = filename;
    // Load file content
    m_content = ofutil::read_file(filename);
    m_modified = false;
    m_undoBuffer.clear();
    m_undoIndex = -1;
    addUndoState(m_content, 0);
}

void ofui::ScriptWindow::save()
{
    if (!m_filename.empty())
    {
        std::ofstream file(m_filename);
        if (file.is_open())
        {
            file << m_content;
            file.close();
            m_modified = false;
        }
    }
}

void ofui::ScriptWindow::setView(FemViewWindow *view)
{
    m_view = view;
}

void ScriptWindow::setContent(const std::string &newContent)
{
    m_content = newContent;
    // Make sure to update the input buffer at the next render cycle
}

bool ofui::ScriptWindow::isEditorActive()
{
    return m_textEditorActive && visible();
}

void ScriptWindow::addUndoState(const std::string &text, int cursorPos)
{
    // Remove states after current undo index if we're in the middle of the undo stack
    if (m_undoIndex < m_undoBuffer.size() - 1)
    {
        m_undoBuffer.resize(m_undoIndex + 1);
    }

    // Add new state
    m_undoBuffer.push_back({text, cursorPos});

    // Limit the size of the undo buffer
    if (m_undoBuffer.size() > maxUndoStates)
    {
        m_undoBuffer.erase(m_undoBuffer.begin());
    }

    m_undoIndex = m_undoBuffer.size() - 1;
}

bool ScriptWindow::undo()
{
    if (m_undoIndex > 0)
    {
        m_undoIndex--;
        m_content = m_undoBuffer[m_undoIndex].content;
        return true;
    }
    return false;
}

bool ScriptWindow::redo()
{
    if (m_undoIndex < m_undoBuffer.size() - 1)
    {
        m_undoIndex++;
        m_content = m_undoBuffer[m_undoIndex].content;
        return true;
    }
    return false;
}

int ScriptWindow::handleInputTextCallback(ImGuiInputTextCallbackData *data)
{
    switch (data->EventFlag)
    {
    case ImGuiInputTextFlags_CallbackAlways:
        // Called every update - could be used for syntax highlighting
        break;
    }
    return 0;
}

void ofui::ScriptWindow::doDraw()
{
    // Menu bar
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Run", "Ctrl+R"))
            {
                if (m_view)
                    m_view->runScript(m_filename);
            }
            if (ImGui::MenuItem("Save", "Ctrl+S"))
            {
                if (m_filename.empty())
                {
                    // Open save dialog
                    m_saveDialog = true;
                }
                else
                    save();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Close"))
            {
                setVisible(false);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "", false, m_undoIndex > 0))
            {
                undo();
            }
            if (ImGui::MenuItem("Redo", "", false, m_undoIndex < m_undoBuffer.size() - 1))
            {
                redo();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", ""))
            {
                ImGui::LogToClipboard();
                ImGui::LogText("%s", m_content.c_str());
                ImGui::LogFinish();
                m_content = "";
                m_modified = true;
            }
            if (ImGui::MenuItem("Copy", ""))
            {
                ImGui::LogToClipboard();
                ImGui::LogText("%s", m_content.c_str());
                ImGui::LogFinish();
            }
            if (ImGui::MenuItem("Paste", ""))
            {
                // Get clipboard text
                const char *clipboardText = ImGui::GetClipboardText();
                if (clipboardText)
                {
                    m_content = clipboardText;
                    m_modified = true;
                    addUndoState(m_content, 0);
                }
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            ImGui::MenuItem("Line Numbers", NULL, &m_showLineNumbers);
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    if (ImGui::Button("Save"))
    {
        if (m_filename.empty())
        {
            // Open save dialog
            m_saveDialog = true;
        }
        else
            save();
    }

    ImGui::SameLine();

    if (ImGui::Button("Run"))
    {
        if (m_view)
            m_view->runScriptFromText(m_content);
    }

    ImGui::SameLine();

    if (ImGui::Button("Clear"))
    {
        if (m_view)
            m_view->newModel();
    }

    ImGui::SameLine();

    if (ImGui::Button("Close"))
    {
        setVisible(false);
    }

    // Status bar
    ImGui::Text("%s%s | Line: %d Col: %d", m_filename.c_str(), m_modified ? "*" : "", 0, 0);
    // ImGui::Text("Line: %d Col: %d", 0, 0);

    // Main editing area
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

    ImVec2 contentSize = ImGui::GetContentRegionAvail();
    contentSize.y -= ImGui::GetTextLineHeightWithSpacing(); // Leave room for status bar

    // Line numbers panel
    if (m_showLineNumbers)
    {
        ImGui::BeginChild("line_numbers", ImVec2(50, contentSize.y), true);
        int lineCount = std::count(m_content.begin(), m_content.end(), '\n') + 1;
        for (int i = 1; i <= lineCount; i++)
        {
            ImGui::Text("%d", i);
        }
        ImGui::EndChild();
        ImGui::SameLine();
        contentSize.x -= 50;
    }

    // Text editor
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.1f, 0.1f, 1.0f)); // Dark background

    // Set flags for more editor-like behavior
    // flags |= ImGuiInputTextFlags_CallbackCompletion;
    // flags |= ImGuiInputTextFlags_CallbackHistory;
    flags |= ImGuiInputTextFlags_CallbackAlways;
    flags |= ImGuiInputTextFlags_CallbackCharFilter;

    // Store previous content for undo detection
    std::string prevContent = m_content;

    // Copy content to the pre-allocated buffer
    strncpy(m_inputBuffer, m_content.c_str(), BUFFER_SIZE - 1);
    m_inputBuffer[BUFFER_SIZE - 1] = 0; // Ensure null termination

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);

    if (ImGui::InputTextMultiline("##source", m_inputBuffer, BUFFER_SIZE, ImVec2(contentSize.x, contentSize.y), flags,
                                  InputTextCallback, this))
    {
        // Update content with the modified buffer
        m_content = m_inputBuffer;
        m_modified = true;

        // Check if content has changed significantly for undo state
        if (m_content != prevContent &&
            (m_content.length() < prevContent.length() * 0.95 || m_content.length() > prevContent.length() * 1.05))
        {
            // Store undo without cursor position (using 0)
            // This avoids the need to access ImGui internals which might change between versions
            addUndoState(m_content, 0);
        }
    }

    ImGui::PopFont();

    m_textEditorActive = ImGui::IsItemActive();

    ImGui::PopStyleColor();
    ImGui::PopStyleVar(2);

    // Save dialog

    if (m_saveDialog)
    {
        IGFD::FileDialogConfig config;
        config.path = ofutil::get_config_value("last_script_dir", ofutil::samples_folder());

        auto viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
        ImVec2 work_size = viewport->WorkSize;
        ImGui::SetNextWindowPos(ImVec2(work_pos.x + 100.0, work_pos.y + 20.0), ImGuiCond_Always);

        ImGuiFileDialog::Instance()->OpenDialog("Save script", "Choose File", ".chai", config);
    }

    if (ImGuiFileDialog::Instance()->Display("Save script", ImGuiWindowFlags_NoCollapse, ImVec2(600, 400)))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            ofutil::set_config_value("last_script_dir", filePath);

            if (filePathName != "")
            {
                m_filename = filePathName;
                save();
            }
        }

        ImGuiFileDialog::Instance()->Close();
        m_saveDialog = false;
    }
}

void ofui::ScriptWindow::doPreDraw()
{}
