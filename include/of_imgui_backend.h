#pragma once

//
// Which Dear ImGui renderer backend ObjectiveFrame uses.
//
// The OpenGL2 backend is fixed function throughout -- glVertexPointer, the
// matrix stacks, glEnableClientState -- and therefore cannot run in a core
// profile at all. The OpenGL3 backend is the one the core profile port needs.
//
// Both are compiled, and the calls go through the thin wrappers below, so the
// two can be compared against each other while the port settles. Define
// OF_IMGUI_BACKEND_GL2 to go back to the old one.
//
// Note that the backends have to match the Dear ImGui version in use: 1.92
// moved dynamic texture updates into the renderer backend, and a backend from
// before that renders nothing against it.
//

#ifndef OF_IMGUI_BACKEND_GL2
#define OF_IMGUI_BACKEND_GL3
#endif

#include <imgui.h>

#ifdef OF_IMGUI_BACKEND_GL3
#include <imgui_impl_opengl3.h>
#else
#include <imgui_impl_opengl2.h>
#endif

namespace ofui {

/** Initialises the renderer backend. */
inline bool imguiBackendInit()
{
#ifdef OF_IMGUI_BACKEND_GL3
    // 150 is the GLSL version that goes with an OpenGL 3.2 core context, which
    // is what this application is heading for.
    return ImGui_ImplOpenGL3_Init("#version 150");
#else
    return ImGui_ImplOpenGL2_Init();
#endif
}

/** Shuts the renderer backend down and releases its GL objects. */
inline void imguiBackendShutdown()
{
#ifdef OF_IMGUI_BACKEND_GL3
    ImGui_ImplOpenGL3_Shutdown();
#else
    ImGui_ImplOpenGL2_Shutdown();
#endif
}

/** Starts a new backend frame. */
inline void imguiBackendNewFrame()
{
#ifdef OF_IMGUI_BACKEND_GL3
    ImGui_ImplOpenGL3_NewFrame();
#else
    ImGui_ImplOpenGL2_NewFrame();
#endif
}

/** Draws what ImGui recorded this frame. */
inline void imguiBackendRenderDrawData(ImDrawData *drawData)
{
#ifdef OF_IMGUI_BACKEND_GL3
    ImGui_ImplOpenGL3_RenderDrawData(drawData);
#else
    ImGui_ImplOpenGL2_RenderDrawData(drawData);
#endif
}

} // namespace ofui
