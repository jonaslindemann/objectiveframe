#pragma once

#include <memory>
#include <string>

#include <imgui.h>

namespace ofui {

enum class PopupResult {
    NONE,
    OK,
    CANCEL,
    YES,
    NO
};

class PopupWindow {
public:
private:
    std::string m_name;
    bool m_visible;
    bool m_modal;
    ImGuiWindowFlags m_windowFlags;
    bool m_updatePos;
    int m_corner;
    bool m_closed;
    PopupResult m_modalResult;

public:
    PopupWindow(const std::string name, bool modal);
    virtual ~PopupWindow();

    static std::shared_ptr<PopupWindow> create(const std::string name, bool modal = true);

    void draw();

    void setWindowFlags(ImGuiWindowFlags flags);
    ImGuiWindowFlags windowFlags();

    void setName(std::string name);
    std::string name();

    void setVisible(bool flag);
    bool visible();
    void show();
    void hide();
    void setSize(int w, int h);
    void align(int corner);
    bool closed();
    void close(PopupResult modalResult);
    PopupResult modalResult();

protected:
    virtual void doPreDraw();
    virtual void doDraw();
    virtual void doPostDraw();

    virtual void doPopup();
};

typedef std::shared_ptr<PopupWindow> PopupWindowPtr;

} // namespace ofui
