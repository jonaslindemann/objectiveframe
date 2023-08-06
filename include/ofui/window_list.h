#pragma once

#include <memory>
#include <string>
#include <vector>

#include <imgui.h>

#include <ofui/ui_window.h>

namespace ofui {

class WindowList {
private:
    std::vector<UiWindowPtr> m_windows;

public:
    WindowList();
    virtual ~WindowList();

    static std::shared_ptr<WindowList> create();

    void add(UiWindowPtr window);
    void clear();
    void hideAll();
    void showAll();

    void draw();
};

typedef std::shared_ptr<WindowList> WindowListPtr;

} // namespace ofui
