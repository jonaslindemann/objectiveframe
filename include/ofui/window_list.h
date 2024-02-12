#pragma once

#include <memory>
#include <string>
#include <vector>

#include <imgui.h>

#include <ofui/ui_window.h>

class FemViewWindow;

namespace ofui {

class WindowList {
private:
    std::vector<UiWindowPtr> m_windows;

    FemViewWindow *m_view;

public:
    WindowList();
    virtual ~WindowList();

    static std::shared_ptr<WindowList> create();

    void setFemView(FemViewWindow *view);

    void findNextPosition(int &x, int &y);
    void placeWindow(UiWindowPtr window);

    void add(UiWindowPtr window);
    void clear();
    void hideAll();
    void showAll();

    void draw();
};

typedef std::shared_ptr<WindowList> WindowListPtr;

} // namespace ofui
