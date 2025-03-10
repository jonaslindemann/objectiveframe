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

class ScriptWindow : public UiWindow {
private:
    FemViewWindow *m_view;

    std::string m_content;
    std::string m_filename;
    bool m_modified = false;
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput; // | ImGuiInputTextFlags_CtrlEnterForNewLine;

    // Fixed-size buffer for ImGui input - avoids heap allocations in render loop
    static const size_t BUFFER_SIZE = 1024 * 1024; // 1MB buffer, adjust as needed
    char m_inputBuffer[BUFFER_SIZE];

    // Line numbers
    bool m_showLineNumbers = true;

    bool m_textEditorActive = false;

    // For undo/redo functionality
    struct UndoState {
        std::string content;
        int cursorPosition; // Note: Currently not being used in the implementation
    };

    std::vector<UndoState> m_undoBuffer;
    int m_undoIndex = -1;
    const int maxUndoStates = 100;

    void addUndoState(const std::string &text, int cursorPos);

    bool undo();
    bool redo();

    // Callback for ImGui InputTextMultiline
    static int InputTextCallback(ImGuiInputTextCallbackData *data)
    {
        ScriptWindow *editor = (ScriptWindow *)data->UserData;
        return editor->handleInputTextCallback(data);
    }

    int handleInputTextCallback(ImGuiInputTextCallbackData *data);

public:
    ScriptWindow(const std::string name);

    static std::shared_ptr<ScriptWindow> create(const std::string name);

    void open(const std::string &filename);
    void save();

    void setView(FemViewWindow *view);

    virtual void doDraw() override;
    virtual void doPreDraw() override;

    // Set content methods
    void setContent(const std::string &newContent);
    const std::string &getContent() const
    {
        return m_content;
    }

    bool isEditorActive();
};

typedef std::shared_ptr<ScriptWindow> ScriptWindowPtr;

} // namespace ofui
