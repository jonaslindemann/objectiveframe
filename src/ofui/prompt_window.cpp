#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4005) // macro redefinition
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <ofui/prompt_window.h>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#include <ofutil/util_functions.h>

#include <FemView.h>

#include <cstring> // For strncpy
#include <fstream>
#include <algorithm>
#include <mutex>

using namespace ofui;

PromptWindow::PromptWindow(const std::string name) : UiWindow(name), m_view(nullptr), m_autoRunScript(true),
    m_selectedCategoryIndex(-1), m_selectedExampleIndex(-1), m_currentDifficultyFilter("All"), m_switchToGenerateTab(false)
{
    setWindowFlags(ImGuiWindowFlags_None);
    setAutoPlacement(true);

    memset(m_inputBuffer, 0, BUFFER_SIZE);
    memset(m_outputBuffer, 0, BUFFER_SIZE);
    memset(m_errorBuffer, 0, BUFFER_SIZE);
    memset(m_searchBuffer, 0, sizeof(m_searchBuffer));

    this->setSize(850, 400);
}

std::shared_ptr<PromptWindow> ofui::PromptWindow::create(const std::string name)
{
    return std::make_shared<PromptWindow>(name);
}

void ofui::PromptWindow::setView(FemViewWindow *view)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_view = view;
}

std::string ofui::PromptWindow::prompt()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::string prompt = m_inputBuffer;
    m_prompt = prompt;
    return m_prompt;
}

std::string ofui::PromptWindow::output()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::string output = m_outputBuffer;
    m_output = output;
    return m_output;
}

std::string ofui::PromptWindow::error()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::string error = m_errorBuffer;
    m_error = error;
    return m_error;
}

void ofui::PromptWindow::addOutput(const std::string &output)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_output += output;
    strncpy(m_outputBuffer, m_output.c_str(), BUFFER_SIZE - 1);
    m_outputBuffer[BUFFER_SIZE - 1] = '\0';
    updateLineOffsets(); // Called while holding lock
}

void ofui::PromptWindow::addError(const std::string &error)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_error += error;
    strncpy(m_errorBuffer, m_error.c_str(), BUFFER_SIZE - 1);
    m_errorBuffer[BUFFER_SIZE - 1] = '\0';
}

void ofui::PromptWindow::clearOutput()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_output = "";
    memset(m_outputBuffer, 0, BUFFER_SIZE);
    m_lineOffsets.clear();
}

void ofui::PromptWindow::clearPrompt()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_prompt = "";
    memset(m_inputBuffer, 0, BUFFER_SIZE);
}

void ofui::PromptWindow::clearError()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_error = "";
    memset(m_errorBuffer, 0, BUFFER_SIZE);
}

void ofui::PromptWindow::clear()
{
    // Single lock to avoid deadlock - don't call other clear methods
    std::lock_guard<std::mutex> lock(m_mutex);
    
    m_output = "";
    memset(m_outputBuffer, 0, BUFFER_SIZE);
    m_lineOffsets.clear();
    
    m_prompt = "";
    memset(m_inputBuffer, 0, BUFFER_SIZE);
}

void ofui::PromptWindow::updateLineOffsets()
{
    // NOTE: Must be called while m_mutex is already locked!
    m_lineOffsets.clear();
    m_lineOffsets.push_back(0);
    
    const char* buf = m_outputBuffer;
    for (int i = 0; buf[i] != '\0'; i++)
    {
        if (buf[i] == '\n')
            m_lineOffsets.push_back(i + 1);
    }
}

void ofui::PromptWindow::renderTextWithLineNumbers(const char* label, char* buffer, const ImVec2& size, ImGuiInputTextFlags inputFlags)
{
    const float lineNumberWidth = 50.0f;
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Update line offsets before rendering (with lock)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        updateLineOffsets();
    }
    
    ImGui::BeginGroup();
    
    // Get cursor position for drawing line numbers
    ImVec2 cursorScreenPos = ImGui::GetCursorScreenPos();
    
    // Create the text input with left padding for line numbers
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(lineNumberWidth, style.FramePadding.y));
    
    ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
    
    // Get the font being used for accurate line height calculation
    ImFont* font = ImGui::GetFont();
    float fontSize = ImGui::GetFontSize();
    
    // DON'T lock here - ImGui needs to access the buffer without locks
    ImGui::InputTextMultiline(label, buffer, BUFFER_SIZE, size, inputFlags);
    
    // Get the scroll position from the InputText state
    float scrollY = 0.0f;
    ImGuiInputTextState* state = ImGui::GetInputTextState(ImGui::GetID(label));
    if (state != nullptr)
    {
        scrollY = state->Scroll.y;
    }
    
    ImGui::PopFont();
    ImGui::PopStyleVar();
    
    // Draw line numbers overlay
    if (ImGui::IsItemVisible())
    {
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
        
        float lineHeight = fontSize;
        
        ImVec2 textPos = cursorScreenPos;
        textPos.x += 5.0f;
        textPos.y += style.FramePadding.y;
        
        // Copy line offsets with lock
        std::vector<int> lineOffsetsCopy;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            lineOffsetsCopy = m_lineOffsets;
        }
        
        int lineCount = (int)lineOffsetsCopy.size();
        ImU32 lineNumberColor = ImGui::GetColorU32(ImGuiCol_TextDisabled);
        
        int firstVisibleLine = (int)(scrollY / lineHeight);
        int lastVisibleLine = (int)((scrollY + size.y) / lineHeight) + 2;
        
        firstVisibleLine = (firstVisibleLine > 0) ? firstVisibleLine : 0;
        
        for (int i = firstVisibleLine; i < lineCount && i < lastVisibleLine; i++)
        {
            char lineNumStr[16];
            snprintf(lineNumStr, sizeof(lineNumStr), "%d", i + 1);
            
            ImVec2 linePos = ImVec2(textPos.x, textPos.y + (i * lineHeight) - scrollY);
            drawList->AddText(linePos, lineNumberColor, lineNumStr);
        }
        
        ImGui::PopFont();
    }
    
    ImGui::EndGroup();
}

void ofui::PromptWindow::loadPromptIntoInput(const std::string& prompt)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    strncpy(m_inputBuffer, prompt.c_str(), BUFFER_SIZE - 1);
    m_inputBuffer[BUFFER_SIZE - 1] = '\0';
    m_prompt = prompt;
}

void ofui::PromptWindow::renderGenerateTab()
{
    bool isProcessing = false;
    bool hasResponse = false;
    bool hasError = false;
    
    // Read buffer states without holding lock during UI operations
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        hasResponse = strlen(m_outputBuffer) > 0;
        hasError = strlen(m_errorBuffer) > 0;
    }

    if (m_view != nullptr)
    {
        isProcessing = m_view->isProcessingAiRequest();
    }

    ImVec2 contentSize = ImGui::GetContentRegionAvail();
    
    if (isProcessing)
    {
        ImGui::ProgressBar(-1.0f * (float)ImGui::GetTime(), ImVec2(contentSize.x, 0.0f), "Processing...");
        ImGui::Dummy(ImVec2(0.0, 10.0));
        
        // Display input buffer (make a copy to avoid holding lock)
        std::string inputCopy;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            inputCopy = m_inputBuffer;
        }
        ImGui::Text("%s", inputCopy.c_str());
    }
    else
    {
        ImGui::Text("Prompt:");

        float promptHeight;
        if (hasResponse || hasError)
        {
            promptHeight = contentSize.y * 0.25f;
        }
        else
        {
            promptHeight = contentSize.y - ImGui::GetTextLineHeightWithSpacing() * 3;
        }

        // Access buffer directly - ImGui handles it
        if (ImGui::InputTextMultiline("##prompt", m_inputBuffer, BUFFER_SIZE, ImVec2(contentSize.x, promptHeight), flags))
        {}

        float buttonWidth = contentSize.x * 0.14f;
        
        if (ImGui::Button("Clear prompt", ImVec2(buttonWidth, 0)))
        {
            clearPrompt();
        }

        ImGui::SameLine();

        if (ImGui::Button("Clear model", ImVec2(buttonWidth, 0)))
        {
            if (m_view)
            {
                m_view->newModel();
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Generate", ImVec2(contentSize.x * 0.2f, 0)))
        {
            if (m_view)
            {
                clearOutput();
                clearError();
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

        if (ImGui::Button("Run", ImVec2(buttonWidth, 0)))
        {
            if (m_view)
            {
                clearError();
                m_view->runScriptFromText(output());
            }
        }
    }

    if (hasResponse)
    {
        ImGui::Dummy(ImVec2(0.0, 10.0));
        ImGui::Separator();
        ImGui::Text("Response:");

        ImVec2 remainingSize = ImGui::GetContentRegionAvail();
        float responseHeight = hasError ? remainingSize.y * 0.6f : remainingSize.y;
        
        renderTextWithLineNumbers("##output", m_outputBuffer, ImVec2(-1, responseHeight), flags);
    }

    if (hasError)
    {
        ImGui::Dummy(ImVec2(0.0, 10.0));
        ImGui::Separator();
        ImGui::Text("Error:");

        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);

        ImVec2 remainingSize = ImGui::GetContentRegionAvail();
        
        if (ImGui::InputTextMultiline("##error", m_errorBuffer, BUFFER_SIZE, ImVec2(-1, remainingSize.y), flags))
        {}

        ImGui::PopFont();
    }
}

void ofui::PromptWindow::renderPromptBrowser()
{
    if (!m_view)
        return;

    auto& promptDb = m_view->getPromptDatabase();
    
    if (!promptDb.isLoaded())
    {
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Prompt database not loaded");
        return;
    }

    ImGui::Text("Search:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(300.0f);
    
    // Access search buffer directly for ImGui
    if (ImGui::InputText("##search", m_searchBuffer, sizeof(m_searchBuffer)))
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_selectedExampleIndex = -1;
    }

    ImGui::SameLine();

    const char* difficulties[] = { "All", "beginner", "intermediate", "advanced", "expert" };
    static int currentDifficulty = 0;
    
    ImGui::Text("Difficulty:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(150.0f);
    if (ImGui::Combo("##difficulty", &currentDifficulty, difficulties, IM_ARRAYSIZE(difficulties)))
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_currentDifficultyFilter = difficulties[currentDifficulty];
        m_selectedExampleIndex = -1;
    }

    ImGui::Separator();

    ImVec2 contentSize = ImGui::GetContentRegionAvail();

    ImGui::BeginChild("LeftPane", ImVec2(250, contentSize.y - 50), true);
    
    ImGui::Text("Categories:");
    ImGui::Separator();

    auto categories = promptDb.getCategories();
    
    for (size_t i = 0; i < categories.size(); i++)
    {
        const auto& category = categories[i];
        
        int selectedIndex;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            selectedIndex = m_selectedCategoryIndex;
        }
        
        bool isSelected = (selectedIndex == static_cast<int>(i));
        if (ImGui::Selectable(category.name.c_str(), isSelected))
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_selectedCategoryIndex = static_cast<int>(i);
            m_selectedExampleIndex = -1;
        }
        
        if (isSelected)
        {
            ImGui::SetItemDefaultFocus();
        }
    }

    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("RightPane", ImVec2(0, contentSize.y - 50), true);
    
    std::vector<ofai::PromptExample> examples;
    
    // Get examples based on filters
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (strlen(m_searchBuffer) > 0)
        {
            examples = promptDb.searchExamplesByName(m_searchBuffer);
        }
        else if (m_selectedCategoryIndex >= 0 && m_selectedCategoryIndex < static_cast<int>(categories.size()))
        {
            examples = promptDb.getExamplesByCategory(categories[m_selectedCategoryIndex].id);
        }
        else
        {
            examples = promptDb.getAllExamples();
        }

        if (m_currentDifficultyFilter != "All")
        {
            examples.erase(
                std::remove_if(examples.begin(), examples.end(),
                    [this](const ofai::PromptExample& ex) {
                        return ex.difficulty != m_currentDifficultyFilter;
                    }),
                examples.end()
            );
        }
    }

    ImGui::Text("Examples (%zu):", examples.size());
    ImGui::Separator();

    for (size_t i = 0; i < examples.size(); i++)
    {
        const auto& example = examples[i];
        
        int selectedExampleIndex;
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            selectedExampleIndex = m_selectedExampleIndex;
        }
        
        bool isSelected = (selectedExampleIndex == static_cast<int>(i));
        
        ImGui::PushID(static_cast<int>(i));
        
        if (ImGui::Selectable(example.name.c_str(), isSelected))
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_selectedExampleIndex = static_cast<int>(i);
        }
        
        if (isSelected)
        {
            ImGui::SetItemDefaultFocus();
            
            ImGui::Indent();
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Difficulty: %s", example.difficulty.c_str());
            
            if (!example.tags.empty())
            {
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "Tags: ");
                ImGui::SameLine();
                for (size_t t = 0; t < example.tags.size(); t++)
                {
                    if (t > 0) ImGui::SameLine();
                    ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "%s", example.tags[t].c_str());
                    if (t < example.tags.size() - 1)
                    {
                        ImGui::SameLine();
                        ImGui::Text(",");
                    }
                }
            }
            
            ImGui::Separator();
            ImGui::TextWrapped("%s", example.prompt.c_str());
            ImGui::Unindent();
        }
        
        ImGui::PopID();
    }

    ImGui::EndChild();

    ImGui::Separator();
    
    int selectedExampleIndex;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        selectedExampleIndex = m_selectedExampleIndex;
    }
    
    bool canUsePrompt = selectedExampleIndex >= 0 && selectedExampleIndex < static_cast<int>(examples.size());
    
    if (!canUsePrompt)
    {
        ImGui::BeginDisabled();
    }
    
    if (ImGui::Button("Use Selected Prompt", ImVec2(200, 0)))
    {
        if (canUsePrompt)
        {
            loadPromptIntoInput(examples[selectedExampleIndex].prompt);
            clearOutput();
            clearError();
            
            std::lock_guard<std::mutex> lock(m_mutex);
            m_switchToGenerateTab = true;
        }
    }
    
    if (!canUsePrompt)
    {
        ImGui::EndDisabled();
    }
}

void ofui::PromptWindow::doDraw()
{
    if (ImGui::BeginTabBar("PromptTabs", ImGuiTabBarFlags_None))
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_switchToGenerateTab)
            {
                ImGui::SetTabItemClosed("Prompts");
                m_switchToGenerateTab = false;
            }
        }
        
        if (ImGui::BeginTabItem("Generate"))
        {
            renderGenerateTab();
            ImGui::EndTabItem();
        }
        
        if (ImGui::BeginTabItem("Prompts"))
        {
            renderPromptBrowser();
            ImGui::EndTabItem();
        }
        
        ImGui::EndTabBar();
    }
}

void ofui::PromptWindow::doPreDraw()
{}
