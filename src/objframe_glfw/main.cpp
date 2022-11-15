#undef GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>

#include "GLFWApplication.h"
#include "GLFWWindow.h"

#include "IvfViewWindow.h"
#include "FemView.h"

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

int main(void)
{
    glfwSetErrorCallback(error_callback);

    auto app = GLFWApplication::create();

    app->hint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    app->hint(GLFW_CONTEXT_VERSION_MINOR, 2);
    app->hint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    app->hint(GLFW_SAMPLES, 4);

    auto window = FemView::create(1280, 1024, "ObjectiveFrame");

    app->addWindow(window);

    app->loop();
}

//! [code]
