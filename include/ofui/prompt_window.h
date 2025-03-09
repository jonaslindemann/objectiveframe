#pragma once
#include <ofui/ui_window.h>

#include "imgui_internal.h"
#include <string>
#include <vector>
#include <regex>
#include <unordered_map>
#include <iostream>

class FemViewWindow;

namespace ofui {

class PromptWindow : public UiWindow {
private:
    FemViewWindow *m_view;

    static const size_t BUFFER_SIZE = 1024 * 1024; // 1MB buffer, adjust as needed
    char m_inputBuffer[BUFFER_SIZE];

    char m_outputBuffer[BUFFER_SIZE];

    std::string m_prompt;
    std::string m_output;

    ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput; // | ImGuiInputTextFlags_CtrlEnterForNewLine;

public:
    PromptWindow(const std::string name);

    static std::shared_ptr<PromptWindow> create(const std::string name);

    void setView(FemViewWindow *view);

    std::string prompt();
    void addOutput(const std::string &output);

    void clearOutput();
    void clearPrompt();

    void clear();

    virtual void doDraw() override;
    virtual void doPreDraw() override;
};

typedef std::shared_ptr<PromptWindow> PromptWindowPtr;

} // namespace ofui
