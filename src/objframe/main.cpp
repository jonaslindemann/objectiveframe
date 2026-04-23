#undef GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>

#include <filesystem>
#include <iostream>
#include <string>

#include "FemView.h"
#include "GLFWApplication.h"
#include <ofutil/util_functions.h>

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

    std::string fullExePathStr = argv[0];

    namespace fs = std::filesystem;

    fs::path fullExePath(fullExePathStr);
    fs::path progPath = fullExePath.parent_path();
    fs::path logoPath = progPath;
    logoPath.append("images").append("logo.png");

    auto window = FemViewWindow::create(1440, 900, "ObjectiveFrame");
    window->setArguments(argc, argv);
    window->setProgramPath(progPath.string());

#ifdef WIN32
    window->setWindowIcon(logoPath.string());
#endif

    window->maximize();

    app->addWindow(window);

    app->loop();
}
