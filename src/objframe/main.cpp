#undef GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#include <ivf/RenderContext.h>

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

    // A debug context reports what the driver objects to. Worth running with
    // while the core profile port is in progress: a compatibility context
    // accepts most of what core rejects, so this names the calls that will stop
    // working before the switch is thrown rather than after.
    //
    // Opt in with --gldebug; a debug context is not free and there is no reason
    // to pay for it in a normal run.

    bool glDebug = false;
    ivf::RenderProfile renderProfile = ivf::RenderProfile::Core;

    for (int i = 1; i < argc; i++)
    {
        const std::string arg = argv[i];

        if (arg == "--gldebug")
            glDebug = true;
        else if (arg == "--core")
            renderProfile = ivf::RenderProfile::Core;
        else if (arg == "--legacy")
            renderProfile = ivf::RenderProfile::Legacy;
        else if (arg == "--mixed")
            renderProfile = ivf::RenderProfile::Mixed;
    }

    app->hint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    app->hint(GLFW_CONTEXT_VERSION_MINOR, 2);
    app->hint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //app->hint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    app->hint(GLFW_SAMPLES, 4);

    if (glDebug)
        app->hint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

    std::string fullExePathStr = argv[0];

    namespace fs = std::filesystem;

    fs::path fullExePath(fullExePathStr);
    fs::path progPath = fullExePath.parent_path();
    fs::path logoPath = progPath;
    logoPath.append("images").append("logo.png");

    auto window = FemViewWindow::create(1440, 900, "ObjectiveFrame");
    window->setArguments(argc, argv);
    window->setGLDebug(glDebug);
    window->setRenderProfile(renderProfile);
    window->setProgramPath(progPath.string());

#ifdef WIN32
    window->setWindowIcon(logoPath.string());
#endif

    window->maximize();

    app->addWindow(window);

    app->loop();
}
