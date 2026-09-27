#pragma once
#include <ofui/ui_window.h>

namespace ofui {

class ConsoleWindow : public UiWindow {
private:
    ImGuiTextBuffer m_buffer;
    ImGuiTextFilter m_filter;
    ImVector<int> m_lineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
    bool m_autoScroll;           // Keep scrolling if already at the bottom.
    std::weak_ptr<UiWindow> m_anchorWindow;
    bool m_placed{false};  // Aligned with the anchor once; never moved again.
    int m_anchorBottom{-1};
    int m_stableFrames{0};
public:
    ConsoleWindow(const std::string name);

    static std::shared_ptr<ConsoleWindow> create(const std::string name);

    void clear();
    // void log(const char* fmt, ...);
    void log(const std::string message);

    // Centres this window horizontally and bottom-aligns it with the given
    // window's bottom edge, once that window's size has settled - at startup and
    // again after each realign(). In between the console stays where it is, or
    // wherever the user drags it.
    void setAnchorWindow(std::shared_ptr<UiWindow> window);

    // Places the console again, as on startup. Called when the main window is
    // resized, alongside the toolbars being put back in their corners.
    void realign();

    virtual void doDraw() override;
    virtual void doPreDraw() override;
};

typedef std::shared_ptr<ConsoleWindow> ConsoleWindowPtr;

} // namespace ofui
