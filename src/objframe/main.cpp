#include <FL/Fl.H>

#include "ObjframeConfig.h"
#define ADVANCED_GL

#include "MainFrame.h"

#include "StatusOutput.h"

#ifdef WIN32
#include <stdlib.h>
#include <windows.h>
#endif

#ifdef USE_LEAP
#include "LeapListener.h"
#endif

int main(int argc, char** argv)
{
    int width = 1280;
    int height = 1024;

    // so_show();

    //
    // Setup window visuals
    //

    so_print("Main: Setting visual.");
    Fl::visual(FL_DEPTH | FL_DOUBLE | FL_RGB | FL_MULTISAMPLE | FL_ALPHA);

    //
    // Retrieve system colors, if any.
    //

#ifdef __APPLE__
    Fl::scheme("default");
#else
    // Fl::scheme("GTK+");
#endif

#ifdef WIN32
    Fl::set_font(FL_HELVETICA, "Segoe UI");
    // Fl::set_font(FL_HELVETICA, "Tahoma");
#endif

#ifdef __APPLE__
    Fl::set_font(FL_HELVETICA, "Lucida Grande");
#endif
    //
    // Determine program path (and remember it...)
    //

#ifdef WIN32
    std::string fullPath = argv[0];
    std::string::size_type lastDelim = fullPath.rfind("\\");
    std::string progPath = fullPath.substr(0, lastDelim) + "\\";
#else
    std::string fullPath = argv[0];
    std::string::size_type lastDelim = fullPath.rfind("/");
    std::string progPath = fullPath.substr(0, lastDelim) + "/";
#endif

    //
    // Create main window
    //

    so_print("Main: Creating main window.");
    MainFrame* frame = new MainFrame();
    // frame->ivfWorkspace->mode(FL_RGB | FL_ALPHA | FL_DEPTH | FL_MULTISAMPLE | FL_DOUBLE );
    frame->setArguments(argc, argv);
    frame->setProgramPath(progPath);
    frame->show();

        // Setup leap motion
#ifdef USE_LEAP
    Controller controller;
    LeapListener listener;
    controller.addListener(listener);
    listener.setWorkspace(frame->ivfWorkspace);
#endif

    //
    // FLTK main loop
    //

    so_print("Main: Entering main loop.");

    return Fl::run();

    //
    // Cleanup
    //

    delete frame;

#ifdef USE_SPLASH
    delete splash;
#endif

    so_hide();
}
