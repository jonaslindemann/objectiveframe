#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <map>
#include <memory>
#include <string>

class GLFWWindow {
private:
    GLFWwindow *m_window;
    GLFWwindow *m_sharedWindow;
    GLFWmonitor *m_monitor;
    int m_width, m_height;
    std::string m_title;
    int m_mouseButton;
    int m_mouseAction;
    int m_mouseMods;
    int m_mouseX;
    int m_mouseY;

    bool m_shiftDown;
    bool m_ctrlDown;
    bool m_altDown;
    bool m_escQuit;

    int m_currentKey;

public:
    GLFWWindow(int width, int height, const std::string title, GLFWmonitor *monitor = nullptr,
               GLFWwindow *shared = nullptr);
    virtual ~GLFWWindow();

    static std::shared_ptr<GLFWWindow> create(int width, int height, const std::string title,
                                              GLFWmonitor *monitor = nullptr, GLFWwindow *shared = nullptr);

    void makeCurrent();
    bool isClosing();
    void close();
    void swapBuffers();
    void destroy();
    int width();
    int height();
    void getSize(int &width, int &height);
    void setWindowIcon(const std::string filename);
    void maximize();

    void draw();

    GLFWwindow *ref();

    int mouseButton();
    int mouseAction();
    int mouseMods();
    int mouseX();
    int mouseY();
    bool isAnyMouseButtonDown();

    bool isShiftDown();
    bool isCtrlDown();
    bool isAltDown();

    bool useEscQuit();
    void setUseEscQuit(bool flag);

public:
    void doKey(int key, int scancode, int action, int mods);
    void doMousePosition(double x, double y);
    void doMouseButton(int button, int action, int mods);
    void doResize(int width, int height);
    void doDraw();

    virtual void onGlfwKey(int key, int scancode, int action, int mods);
    virtual void onGlfwMousePosition(double x, double y);
    virtual void onGlfwMouseButton(int button, int action, int mods);
    virtual void onGlfwResize(int width, int height);
    virtual void onGlfwDraw();
};

typedef std::shared_ptr<GLFWWindow> GLFWWindowPtr;
