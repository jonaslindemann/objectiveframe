#undef GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#include <ivf/RenderContext.h>

#include <filesystem>
#include <iostream>
#include <string>

#include "FemView.h"
#include "GLFWApplication.h"
#include <ofutil/util_functions.h>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

static void error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

// Absolute path of the running executable. Everything located relative to
// it - data folders, the tetgen helper - must not depend on the working
// directory: the mesher changes it to the temp folder before running tetgen,
// so a relative "." program path would then point there instead.
static std::filesystem::path executablePath(const char *argv0)
{
    namespace fs = std::filesystem;

#ifdef __APPLE__
    // argv[0] is only what the shell was given ("./objframe", or a bare
    // "objframe" found on PATH); dyld knows the real location.
    uint32_t size = 0;
    _NSGetExecutablePath(nullptr, &size);
    std::string buffer(size, '\0');
    if (_NSGetExecutablePath(buffer.data(), &size) == 0)
    {
        std::error_code ec;
        auto path = fs::weakly_canonical(fs::path(buffer.c_str()), ec);
        if (!ec)
            return path;
    }
#endif

    std::error_code ec;
    auto path = fs::absolute(fs::path(argv0), ec);
    return ec ? fs::path(argv0) : path;
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

    namespace fs = std::filesystem;

    fs::path fullExePath = executablePath(argv[0]);
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
