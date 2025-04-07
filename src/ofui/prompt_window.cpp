#include <ofui/prompt_window.h>

#include <ofutil/util_functions.h>

#include <FemView.h>

#include <cstring> // For strncpy
#include <fstream>

using namespace ofui;

PromptWindow::PromptWindow(const std::string name) : UiWindow(name), m_view(nullptr), m_autoRunScript(true)
{
    setWindowFlags(ImGuiWindowFlags_None);
    enableMenuBar();
    setAutoPlacement(true);

    memset(m_inputBuffer, 0, BUFFER_SIZE);  // Clear the input buffer
    memset(m_outputBuffer, 0, BUFFER_SIZE); // Clear the input buffer
    memset(m_errorBuffer, 0, BUFFER_SIZE);  // Clear the input buffer

    this->setSize(800, 730);
}

std::shared_ptr<PromptWindow> ofui::PromptWindow::create(const std::string name)
{
    return std::make_shared<PromptWindow>(name);
}

void ofui::PromptWindow::setView(FemViewWindow *view)
{
    m_view = view;
}

std::string ofui::PromptWindow::prompt()
{
    std::string prompt = m_inputBuffer;
    m_prompt = prompt;
    return m_prompt;
}

std::string ofui::PromptWindow::output()
{
    std::string output = m_outputBuffer;
    m_output = output;
    return m_output;
}

std::string ofui::PromptWindow::error()
{
    std::string error = m_errorBuffer;
    m_error = error;
    return m_error;
}

void ofui::PromptWindow::addOutput(const std::string &output)
{
    m_output += output;
    strncpy(m_outputBuffer, m_output.c_str(), BUFFER_SIZE - 1);
    m_outputBuffer[BUFFER_SIZE - 1] = '\0';
}

void ofui::PromptWindow::addError(const std::string &error)
{
    m_error += error;
    strncpy(m_errorBuffer, m_error.c_str(), BUFFER_SIZE - 1);
    m_errorBuffer[BUFFER_SIZE - 1] = '\0';
}

void ofui::PromptWindow::clearOutput()
{
    m_output = "";
    memset(m_outputBuffer, 0, BUFFER_SIZE);
}

void ofui::PromptWindow::clearPrompt()
{
    m_prompt = "";
    memset(m_inputBuffer, 0, BUFFER_SIZE);
}

void ofui::PromptWindow::clearError()
{
    m_error = "";
    memset(m_errorBuffer, 0, BUFFER_SIZE);
}

void ofui::PromptWindow::clear()
{
    clearOutput();
    clearPrompt();
}

void ofui::PromptWindow::doDraw()
{
    // Menu bar
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Run", "Ctrl+R"))
            {}
            if (ImGui::MenuItem("Save", "Ctrl+S"))
            {}
            ImGui::Separator();
            if (ImGui::MenuItem("Close"))
            {
                setVisible(false);
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImVec2 contentSize = ImGui::GetContentRegionAvail();
    contentSize.y = ImGui::GetTextLineHeightWithSpacing() * 5; // Leave room for status bar

    bool isProcessing = false;

    if (m_view != nullptr)
    {
        isProcessing = m_view->isProcessingAiRequest();
    }

    if (isProcessing)
    {
        ImGui::ProgressBar(-1.0f * (float)ImGui::GetTime(), ImVec2(contentSize.x, 0.0f), "Processing...");
        ImGui::Dummy(ImVec2(0.0, 10.0));
        ImGui::Text(m_inputBuffer);
    }
    else
    {
        ImGui::Text("Prompt:");

        if (ImGui::InputTextMultiline("##prompt", m_inputBuffer, BUFFER_SIZE, ImVec2(contentSize.x, contentSize.y),
                                      flags))
        {}

        if (ImGui::Button("Clear prompt", ImVec2(contentSize.x * 0.2, 0)))
        {
            clear();
        }

        ImGui::SameLine();

        if (ImGui::Button("Clear model", ImVec2(contentSize.x * 0.2, 0)))
        {
            if (m_view)
            {
                m_view->newModel();
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Generate", ImVec2(contentSize.x * 0.2, 0)))
        {
            if (m_view)
            {
                clearOutput();
                m_view->makeAiRequest(prompt());
            }
        }

        ImGui::SameLine();

        if (ImGui::Checkbox("Auto run", &m_autoRunScript))
        {
            if (m_view)
            {
                m_view->setAutoRunAiScript(m_autoRunScript);
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Run", ImVec2(contentSize.x * 0.2, 0)))
        {
            if (m_view)
            {
                m_view->runScriptFromText(output());
            }
        }
    }

    ImGui::Dummy(ImVec2(0.0, 10.0));

    ImGui::Text("Response:");

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);

    if (ImGui::InputTextMultiline("##output", m_outputBuffer, BUFFER_SIZE, ImVec2(-1, contentSize.y * 2), flags))
    {}

    ImGui::PopFont();

    ImGui::Text("Error:");

    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);

    if (ImGui::InputTextMultiline("##error", m_errorBuffer, BUFFER_SIZE, ImVec2(-1, -1), flags))
    {}

    ImGui::PopFont();
}

void ofui::PromptWindow::doPreDraw()
{}
