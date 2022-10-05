#pragma once

#include <memory>
#include <string>

#include <imgui.h>

namespace ofui
{

class UiWindow
{
private:
    std::string m_name;
    bool m_visible;
    ImGuiWindowFlags m_windowFlags;
    bool m_updatePos;
    bool m_centerBottom;

    int m_corner;

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
   
    void align(int corner);
    void centerBottom();

    void setPosition(int x, int y);

protected:
    virtual void doPreDraw();
    virtual void doDraw();
    virtual void doPostDraw();
};

typedef std::shared_ptr<UiWindow> UiWindowPtr;

}
