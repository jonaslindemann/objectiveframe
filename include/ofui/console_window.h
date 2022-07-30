#pragma once
#include <ofui/ui_window.h>

namespace ofui
{

class ConsoleWindow : public UiWindow
{
private:
    ImGuiTextBuffer m_buffer;
    ImGuiTextFilter m_filter;
    ImVector<int> m_lineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
    bool m_autoScroll; // Keep scrolling if already at the bottom.
public:
    ConsoleWindow(const std::string name);

    static std::shared_ptr<ConsoleWindow> create(const std::string name);

    void clear();
    // void log(const char* fmt, ...);
    void log(const std::string message);

    virtual void doDraw() override;
    virtual void doPreDraw() override;
};

typedef std::shared_ptr<ConsoleWindow> ConsoleWindowPtr;

}
