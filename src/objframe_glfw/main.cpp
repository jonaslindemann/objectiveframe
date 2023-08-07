#undef GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>

#include <filesystem>
#include <iostream>
#include <string>

#include "FemView.h"
#include "GLFWApplication.h"

static void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

int main(int argc, char **argv)
{
    glfwSetErrorCallback(error_callback);

    auto app = GLFWApplication::create();

    app->hint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    app->hint(GLFW_CONTEXT_VERSION_MINOR, 2);
    app->hint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    app->hint(GLFW_SAMPLES, 4);

    std::string fullExePath = argv[0];

    std::filesystem::path path(fullExePath);
#ifdef WIN32
    std::string progPath = path.parent_path().string() + "\\";
#else
    std::string progPath = path.parent_path().string() + "/";
#endif

    auto window = FemViewWindow::create(1440, 900, "ObjectiveFrame");
    window->setArguments(argc, argv);
    window->setProgramPath(progPath);
#ifdef WIN32
    window->setWindowIcon(progPath + "images\\logo.png");
#endif
    window->maximize();

    app->addWindow(window);

    app->loop();
}

//! [code]
