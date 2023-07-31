#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <memory>
#include <string>
#include <vector>

#include "GLFWWindow.h"

class GLFWApplication {
private:
    std::vector<GLFWWindowPtr> m_windows;

public:
    GLFWApplication();
    virtual ~GLFWApplication();

    static std::shared_ptr<GLFWApplication> create();

    void addWindow(GLFWWindowPtr window);
    void loop();
    void pollEvents();
    void hint(int hint, int value);
};

typedef std::shared_ptr<GLFWApplication> GLFWApplicationPtr;

class GLFWWindowTracker {
private:
    std::map<GLFWwindow *, GLFWWindowPtr> m_windowMap;

    GLFWWindowTracker(){};

public:
    static GLFWWindowTracker *instance()
    {
        static GLFWWindowTracker m_instance;
        return &m_instance;
    }

    void addWindow(GLFWWindowPtr window);
    GLFWWindowPtr get(GLFWwindow *window);
};
