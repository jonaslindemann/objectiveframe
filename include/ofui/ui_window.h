#pragma once

#include <memory>
#include <string>

#include <imgui.h>

namespace ofui {

class UiWindow {
private:
    std::string m_name;
    bool m_visible;
    ImGuiWindowFlags m_windowFlags;
    bool m_updatePos;
    bool m_centerBottom;
    bool m_center;
    bool m_setPos;
    int m_corner;
    bool m_firstDraw;
    int m_width;
    int m_height;
    bool m_canClose;
    int m_x;
    int m_y;

    bool m_newPos;

public:
    UiWindow(const std::string name);
    virtual ~UiWindow();

    static std::shared_ptr<UiWindow> create(const std::string name);

    void draw();

    void setWindowFlags(ImGuiWindowFlags flags);
    ImGuiWindowFlags windowFlags();

    void setVisible(bool flag);
    bool visible();
    void show();
    void hide();
    void setSize(int w, int h);
    int width();
    int height();

    bool canClose();
    void setCanClose(bool flag);

    void align(int corner);
    void centerBottom();
    void center();

    void setPosition(int x, int y);

    int x();
    int y();

protected:
    virtual void doPreDraw();
    virtual void doDraw();
    virtual void doPostDraw();
};

typedef std::shared_ptr<UiWindow> UiWindowPtr;

} // namespace ofui
