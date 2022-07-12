// dear imgui: Platform Backend for FLTK/FreeFLTK
// This needs to be used along with a Renderer (e.g. OpenGL2)

// !!! FLTK/FreeFLTK IS OBSOLETE PREHISTORIC SOFTWARE. Using FLTK is not recommended unless you really miss the 90's. !!!
// !!! If someone or something is teaching you FLTK today, you are being abused. Please show some resistance. !!!
// !!! Nowadays, prefer using GLFW or SDL instead!

// Implemented features:
//  [X] Platform: Partial keyboard support. Since 1.87 we are using the io.AddKeyEvent() function. Pass ImGuiKey values to all key functions e.g. ImGui::IsKeyPressed(ImGuiKey_Space). [Legacy FLTK values will also be supported unless IMGUI_DISABLE_OBSOLETE_KEYIO is set]
// Issues:
//  [ ] Platform: FLTK is unable to distinguish e.g. Backspace from CTRL+H or TAB from CTRL+I
//  [ ] Platform: Missing mouse cursor shape/visibility support.
//  [ ] Platform: Missing clipboard support (not supported by Glut).
//  [ ] Platform: Missing gamepad support.

// You can use unmodified imgui_impl_* files in your project. See examples/ folder for examples of using this.
// Prefer including the entire imgui/ repository into your project (either as a copy or as a submodule), and only build the backends you need.
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// CHANGELOG
// (minor and older changes stripped away, please see git history for details)
//  2022-01-26: Inputs: replaced short-lived io.AddKeyModsEvent() (added two weeks ago) with io.AddKeyEvent() using ImGuiKey_ModXXX flags. Sorry for the confusion.
//  2022-01-17: Inputs: calling new io.AddMousePosEvent(), io.AddMouseButtonEvent(), io.AddMouseWheelEvent() API (1.87+).
//  2022-01-10: Inputs: calling new io.AddKeyEvent(), io.AddKeyModsEvent() + io.SetKeyEventNativeData() API (1.87+). Support for full ImGuiKey range.
//  2019-04-03: Misc: Renamed imgui_impl_freeglut.cpp/.h to imgui_impl_glut.cpp/.h.
//  2019-03-25: Misc: Made io.DeltaTime always above zero.
//  2018-11-30: Misc: Setting up io.BackendPlatformName so it can be displayed in the About Window.
//  2018-03-22: Added FLTK Platform binding.

#include "imgui.h"
#include "imgui_impl_fltk.h"
#include "imgui_impl_opengl2.h"

#include <Windows.h>

#include <iostream>

#include <glad/glad.h>

using namespace std;

// Platform depenendant includes

#include <FL/Fl.H>

// OpenGL and FLTK includes

#include <FL/Fl_Gl_Window.H>


#ifdef _MSC_VER
#pragma warning (disable: 4505) // unreferenced local function has been removed (stb stuff)
#endif

static DWORD g_Time = 0;          // Current time, in milliseconds
static float g_pixelsPerUnit;

// Glut has 1 function for characters and one for "special keys". We map the characters in the 0..255 range and the keys above.
static ImGuiKey ImGui_ImplFLTK_KeyToImGuiKey(int key)
{
    switch (key)
    {
        case '\t':                      return ImGuiKey_Tab;
        case FL_Left:                   return ImGuiKey_LeftArrow;
        case FL_Right:                  return ImGuiKey_RightArrow;
        case FL_Up:                     return ImGuiKey_UpArrow;
        case FL_Down:                   return ImGuiKey_DownArrow;
        case FL_Page_Up:                return ImGuiKey_PageUp;
        case FL_Page_Down:              return ImGuiKey_PageDown;
        case FL_Home:                   return ImGuiKey_Home;
        case FL_End:                    return ImGuiKey_End;
        case FL_Insert:                 return ImGuiKey_Insert;
        case FL_Delete:                 return ImGuiKey_Delete;
        case FL_BackSpace:              return ImGuiKey_Backspace;
        case ' ':                       return ImGuiKey_Space;
        case 13:                        return ImGuiKey_Enter;
        case 27:                        return ImGuiKey_Escape;
        case 39:                        return ImGuiKey_Apostrophe;
        case 44:                        return ImGuiKey_Comma;
        case 45:                        return ImGuiKey_Minus;
        case 46:                        return ImGuiKey_Period;
        case 47:                        return ImGuiKey_Slash;
        case 59:                        return ImGuiKey_Semicolon;
        case 61:                        return ImGuiKey_Equal;
        case 91:                        return ImGuiKey_LeftBracket;
        case 92:                        return ImGuiKey_Backslash;
        case 93:                        return ImGuiKey_RightBracket;
        case 96:                        return ImGuiKey_GraveAccent;
        //case 0:                         return ImGuiKey_CapsLock;
        //case 0:                         return ImGuiKey_ScrollLock;
        case 256 + 0x006D:              return ImGuiKey_NumLock;
        //case 0:                         return ImGuiKey_PrintScreen;
        //case 0:                         return ImGuiKey_Pause;
        //case '0':                       return ImGuiKey_Keypad0;
        //case '1':                       return ImGuiKey_Keypad1;
        //case '2':                       return ImGuiKey_Keypad2;
        //case '3':                       return ImGuiKey_Keypad3;
        //case '4':                       return ImGuiKey_Keypad4;
        //case '5':                       return ImGuiKey_Keypad5;
        //case '6':                       return ImGuiKey_Keypad6;
        //case '7':                       return ImGuiKey_Keypad7;
        //case '8':                       return ImGuiKey_Keypad8;
        //case '9':                       return ImGuiKey_Keypad9;
        //case 46:                        return ImGuiKey_KeypadDecimal;
        //case 47:                        return ImGuiKey_KeypadDivide;
        case 42:                        return ImGuiKey_KeypadMultiply;
        //case 45:                        return ImGuiKey_KeypadSubtract;
        case 43:                        return ImGuiKey_KeypadAdd;
        //case 13:                        return ImGuiKey_KeypadEnter;
        //case 0:                         return ImGuiKey_KeypadEqual;
        case FL_Control_L:              return ImGuiKey_LeftCtrl;
        case FL_Shift_L:                return ImGuiKey_LeftShift;
        case FL_Alt_L:                  return ImGuiKey_LeftAlt;
        //case 0:                         return ImGuiKey_LeftSuper;
        case FL_Control_R:              return ImGuiKey_RightCtrl;
        case FL_Shift_R:                return ImGuiKey_RightShift;
        case FL_Alt_R:                  return ImGuiKey_RightAlt;
        //case 0:                         return ImGuiKey_RightSuper;
        //case 0:                         return ImGuiKey_Menu;
        case '0':                       return ImGuiKey_0;
        case '1':                       return ImGuiKey_1;
        case '2':                       return ImGuiKey_2;
        case '3':                       return ImGuiKey_3;
        case '4':                       return ImGuiKey_4;
        case '5':                       return ImGuiKey_5;
        case '6':                       return ImGuiKey_6;
        case '7':                       return ImGuiKey_7;
        case '8':                       return ImGuiKey_8;
        case '9':                       return ImGuiKey_9;
        case 'A': case 'a':             return ImGuiKey_A;
        case 'B': case 'b':             return ImGuiKey_B;
        case 'C': case 'c':             return ImGuiKey_C;
        case 'D': case 'd':             return ImGuiKey_D;
        case 'E': case 'e':             return ImGuiKey_E;
        case 'F': case 'f':             return ImGuiKey_F;
        case 'G': case 'g':             return ImGuiKey_G;
        case 'H': case 'h':             return ImGuiKey_H;
        case 'I': case 'i':             return ImGuiKey_I;
        case 'J': case 'j':             return ImGuiKey_J;
        case 'K': case 'k':             return ImGuiKey_K;
        case 'L': case 'l':             return ImGuiKey_L;
        case 'M': case 'm':             return ImGuiKey_M;
        case 'N': case 'n':             return ImGuiKey_N;
        case 'O': case 'o':             return ImGuiKey_O;
        case 'P': case 'p':             return ImGuiKey_P;
        case 'Q': case 'q':             return ImGuiKey_Q;
        case 'R': case 'r':             return ImGuiKey_R;
        case 'S': case 's':             return ImGuiKey_S;
        case 'T': case 't':             return ImGuiKey_T;
        case 'U': case 'u':             return ImGuiKey_U;
        case 'V': case 'v':             return ImGuiKey_V;
        case 'W': case 'w':             return ImGuiKey_W;
        case 'X': case 'x':             return ImGuiKey_X;
        case 'Y': case 'y':             return ImGuiKey_Y;
        case 'Z': case 'z':             return ImGuiKey_Z;
        /*
        case 256 + GLUT_KEY_F1:         return ImGuiKey_F1;
        case 256 + GLUT_KEY_F2:         return ImGuiKey_F2;
        case 256 + GLUT_KEY_F3:         return ImGuiKey_F3;
        case 256 + GLUT_KEY_F4:         return ImGuiKey_F4;
        case 256 + GLUT_KEY_F5:         return ImGuiKey_F5;
        case 256 + GLUT_KEY_F6:         return ImGuiKey_F6;
        case 256 + GLUT_KEY_F7:         return ImGuiKey_F7;
        case 256 + GLUT_KEY_F8:         return ImGuiKey_F8;
        case 256 + GLUT_KEY_F9:         return ImGuiKey_F9;
        case 256 + GLUT_KEY_F10:        return ImGuiKey_F10;
        case 256 + GLUT_KEY_F11:        return ImGuiKey_F11;
        case 256 + GLUT_KEY_F12:        return ImGuiKey_F12;
        */
        default:                        return ImGuiKey_None;
    }
}

bool ImGui_ImplFLTK_Init()
{
    ImGuiIO& io = ImGui::GetIO();

    io.BackendPlatformName = "imgui_impl_fltk";

    g_Time = 0;

    return true;
}

void ImGui_ImplFLTK_InstallFuncs()
{
}

void ImGui_ImplFLTK_Shutdown()
{
}

void ImGui_ImplFLTK_NewFrame()
{
    // Setup time step
    ImGuiIO& io = ImGui::GetIO();
    DWORD current_time = GetTickCount64();
    DWORD delta_time = (current_time - g_Time);
    if (delta_time <= 0)
        delta_time = 1;
    io.DeltaTime = delta_time / 1000.0f;
    g_Time = current_time;

    // Start the frame
    ImGui::NewFrame();
}

static void ImGui_ImplFLTK_UpdateKeyModifiers()
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddKeyEvent(ImGuiKey_ModCtrl, Fl::get_key(FL_Control_L));
    io.AddKeyEvent(ImGuiKey_ModShift, Fl::get_key(FL_Shift_L));
    io.AddKeyEvent(ImGuiKey_ModAlt, Fl::get_key(FL_Alt_L));
}

static void ImGui_ImplFLTK_AddKeyEvent(ImGuiKey key, bool down, int native_keycode)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddKeyEvent(key, down);
    io.SetKeyEventNativeData(key, native_keycode, -1); // To support legacy indexing (<1.87 user code)
}

void ImGui_ImplFLTK_Keyboard()
{
    ImGuiIO& io = ImGui::GetIO();

    int flkey = Fl::event_key();

    if ((Fl::event_key() >= 32)&&(Fl::event_key() < 255))
        io.AddInputCharacter((unsigned int)Fl::event_key());

    ImGuiKey key = ImGui_ImplFLTK_KeyToImGuiKey(Fl::event_key());

    ImGui_ImplFLTK_AddKeyEvent(key, true, Fl::event_key());
    ImGui_ImplFLTK_UpdateKeyModifiers();
}

void ImGui_ImplFLTK_KeyUp()
{
    ImGuiKey key = ImGui_ImplFLTK_KeyToImGuiKey(Fl::event_key());
    ImGui_ImplFLTK_AddKeyEvent(key, false, Fl::event_key());
    ImGui_ImplFLTK_UpdateKeyModifiers();
}

void ImGui_ImplFLTK_SpecialFunc(int key, int x, int y)
{
    //printf("key_down_func %d\n", key);
    ImGuiKey imgui_key = ImGui_ImplFLTK_KeyToImGuiKey(key + 256);
    ImGui_ImplFLTK_AddKeyEvent(imgui_key, true, key + 256);
    ImGui_ImplFLTK_UpdateKeyModifiers();
    (void)x; (void)y; // Unused
}

void ImGui_ImplFLTK_SpecialUpFunc(int key, int x, int y)
{
    //printf("key_up_func %d\n", key);
    ImGuiKey imgui_key = ImGui_ImplFLTK_KeyToImGuiKey(key + 256);
    ImGui_ImplFLTK_AddKeyEvent(imgui_key, false, key + 256);
    ImGui_ImplFLTK_UpdateKeyModifiers();
    (void)x; (void)y; // Unused
}

void ImGui_ImplFLTK_Push(int& button)
{
    ImGuiIO& io = ImGui::GetIO();

    io.AddKeyEvent(ImGuiKey_ModCtrl, Fl::get_key(FL_Control_L));
    io.AddKeyEvent(ImGuiKey_ModShift, Fl::get_key(FL_Shift_L) != 0);
    io.AddKeyEvent(ImGuiKey_ModAlt, Fl::get_key(FL_Alt_L) != 0);

    io.AddMousePosEvent((float)Fl::event_x() * g_pixelsPerUnit, (float)Fl::event_y() * g_pixelsPerUnit);
    button = -1;
    if (Fl::event_button() == FL_LEFT_MOUSE) button = 0;
    if (Fl::event_button() == FL_MIDDLE_MOUSE) button = 1;
    if (Fl::event_button() == FL_RIGHT_MOUSE) button = 2;

    if (button != -1)
        io.AddMouseButtonEvent(button, true);
}

void ImGui_ImplFLTK_Drag()
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent((float)Fl::event_x() * g_pixelsPerUnit, (float)Fl::event_y() * g_pixelsPerUnit);
}

IMGUI_IMPL_API void ImGui_ImplFLTK_Release(int& button)
{
    ImGuiIO& io = ImGui::GetIO();
    if (button!=-1)
        io.AddMouseButtonEvent(button, false);
    button = -1;
}

void ImGui_ImplFLTK_Resize(int w, int h)
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)w, (float)h);
}

void ImGui_ImplFLTK_Move()
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent((float)Fl::event_x()*g_pixelsPerUnit, (float)Fl::event_y() * g_pixelsPerUnit);
}

ImGuiFLTKImpl::ImGuiFLTKImpl()
    :m_button{-1},
     m_firstDraw{true},
     m_initialised{false}
{
    g_pixelsPerUnit = 1.0f;
}

void ImGuiFLTKImpl::setPixelsPerUnit(float ppu)
{
    g_pixelsPerUnit = ppu;
}

bool ImGuiFLTKImpl::isImGuiInitialised()
{
    return m_initialised;
}

bool ImGuiFLTKImpl::isOverWindow()
{
    ImGuiIO& io = ImGui::GetIO();
    return io.WantCaptureMouse;
}

void ImGuiFLTKImpl::doDrawImGui()
{
    ImGuiIO& io = ImGui::GetIO();

    //glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);

    if (m_firstDraw)
    {
        m_firstDraw = false;
        ImGui_ImplOpenGL2_NewFrame();
        ImGui_ImplFLTK_NewFrame();
        onDrawImGui();
        ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    }

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplFLTK_NewFrame();

    onDrawImGui();

    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiFLTKImpl::doInitImGui(int w, int h)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    onInitImGui();

    io.DisplaySize = ImVec2((float)w, (float)h);

    ImGui_ImplFLTK_Init();
    ImGui_ImplOpenGL2_Init();

    m_initialised = true;
}

void ImGuiFLTKImpl::doImGuiResize(int w, int h)
{
    ImGui_ImplFLTK_Resize(w, h);
}

void ImGuiFLTKImpl::doImGuiMove()
{
    ImGui_ImplFLTK_Move();
}

void ImGuiFLTKImpl::doImGuiPush()
{
    ImGui_ImplFLTK_Push(m_button);
}

void ImGuiFLTKImpl::doImGuiDrag()
{
    ImGui_ImplFLTK_Drag();
}

void ImGuiFLTKImpl::doImGuiRelease()
{
    ImGui_ImplFLTK_Release(m_button);
}

void ImGuiFLTKImpl::doImGuiKeyboard()
{
    ImGui_ImplFLTK_Keyboard();
}

void ImGuiFLTKImpl::doImGuiKeyUp()
{
    ImGui_ImplFLTK_KeyUp();
}

void ImGuiFLTKImpl::onDrawImGui()
{
}

void ImGuiFLTKImpl::onInitImGui()
{
}
