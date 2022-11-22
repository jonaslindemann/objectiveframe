#undef GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>

#include <string>
#include <iostream>
#include <filesystem>

#include "GLFWApplication.h"
#include "GLFWWindow.h"

#include "IvfViewWindow.h"
#include "FemView.h"

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

int main(int argc, char** argv)
{
    glfwSetErrorCallback(error_callback);

    auto app = GLFWApplication::create();

    app->hint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    app->hint(GLFW_CONTEXT_VERSION_MINOR, 2);
    app->hint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    app->hint(GLFW_SAMPLES, 4);

    std::string fullExePath = argv[0];

    std::filesystem::path path(fullExePath);
    std::string progPath = path.parent_path().string() + "\\";

    auto window = FemViewWindow::create(1280, 1024, "ObjectiveFrame");
    window->setProgramPath(progPath);

    app->addWindow(window);

    app->loop();
}

//! [code]
