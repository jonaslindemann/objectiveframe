#include "GLFWWindow.h"

#undef GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>

#include <iostream>

using namespace std;

std::shared_ptr<GLFWWindow> GLFWWindow::create(int width, int height, const std::string title, GLFWmonitor* monitor, GLFWwindow* shared)
{
    return GLFWWindowPtr(new GLFWWindow(width, height, title, monitor, shared));
}

GLFWWindow::GLFWWindow(int width, int height, const std::string title, GLFWmonitor* monitor, GLFWwindow* shared)
    : m_width(width)
    , m_height(height)
    , m_title(title)
    , m_mouseButton(-1)
    , m_mouseAction(-1)
    , m_mouseMods(-1)
    , m_mouseX(-1)
    , m_mouseY(-1)
{
    m_window = glfwCreateWindow(width, height, title.c_str(), monitor, shared);

    if (!m_window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
}

GLFWWindow::~GLFWWindow()
{
    if (m_window)
        glfwDestroyWindow(m_window);
}

void GLFWWindow::makeCurrent()
{
    if (m_window)
    {
        glfwMakeContextCurrent(m_window);
        gladLoadGL();
        glfwSwapInterval(1);
    }
}

bool GLFWWindow::isClosing()
{
    if (m_window)
        return glfwWindowShouldClose(m_window);
    else
        return true;
}

void GLFWWindow::close()
{
    glfwSetWindowShouldClose(m_window, GLFW_TRUE);
}

void GLFWWindow::swapBuffers()
{
    if (m_window)
        glfwSwapBuffers(m_window);
}

void GLFWWindow::destroy()
{
    if (m_window)
    {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
}

GLFWwindow* GLFWWindow::ref()
{
    return m_window;
}

int GLFWWindow::mouseButton()
{
    return m_mouseButton;
}

int GLFWWindow::mouseAction()
{
    return m_mouseAction;
}

int GLFWWindow::mouseMods()
{
    return m_mouseMods;
}

int GLFWWindow::mouseX()
{
    return m_mouseX;
}

int GLFWWindow::mouseY()
{
    return m_mouseY;
}

int GLFWWindow::width()
{
    if (m_window)
    {
        glfwGetFramebufferSize(m_window, &m_width, &m_height);
        return m_width;
    }
    else
        return -1;
}

int GLFWWindow::height()
{
    if (m_window)
    {
        glfwGetFramebufferSize(m_window, &m_width, &m_height);
        return m_height;
    }
    else
        return -1;
}

void GLFWWindow::getSize(int& width, int& height)
{
    if (m_window)
    {
        glfwGetFramebufferSize(m_window, &m_width, &m_height);
        width = m_width;
        height = m_height;
    }
    else
    {
        width = -1;
        height = -1;
    }
}

void GLFWWindow::doKey(int key, int scancode, int action, int mods)
{
    cout << "doKey: " << key << ", " << scancode << ", " << action << ", " << mods << endl;
    onGlfwKey(key, scancode, action, mods);
}

void GLFWWindow::doMousePosition(double x, double y)
{
    cout << "doMousePosition: " << x << ", " << y << endl;
    m_mouseX = x;
    m_mouseY = y;
    onGlfwMousePosition(x, y);
}

void GLFWWindow::doMouseButton(int button, int action, int mods)
{
    cout << "doMouseButton: " << button << ", " << action << ", " << mods << endl;
    m_mouseButton = button;
    m_mouseAction = action;
    m_mouseMods = mods;
    onGlfwMouseButton(button, action, mods);
}

void GLFWWindow::doResize(int width, int height)
{
    cout << "doResize: " << width << ", " << height << endl;
    onGlfwResize(width, height);
}

void GLFWWindow::doDraw()
{
    int width, height;

    this->getSize(width, height);

    const float ratio = width / (float)height;

    glViewport(0, 0, width, height);

    onGlfwDraw();
}

void GLFWWindow::onGlfwKey(int key, int scancode, int action, int mods)
{
}

void GLFWWindow::onGlfwMousePosition(double x, double y)
{
}

void GLFWWindow::onGlfwMouseButton(int button, int action, int mods)
{
}

void GLFWWindow::onGlfwResize(int width, int height)
{
}

void GLFWWindow::onGlfwDraw()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

