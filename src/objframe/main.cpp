#include <FL/Fl.H>

#define ADVANCED_GL

#ifdef ADVANCED_GL
#include "MainFrame.h"
#else
#include "MainFrame_lt.h"
#endif

#include "SplashFrame.h"

#include "StatusOutput.h"

#ifdef WIN32
#include <stdlib.h>
#include <windows.h>
#endif

#include "LeapListener.h"

int
main(int argc, char **argv)
{
    bool fullscreen = false;

    int width = 1280;
    int height = 1024;

#ifdef WIN32

    bool switchResolution = false;

    int bits = 16;
    int freq = 100;

    if ((fullscreen)&&(switchResolution))						// Attempt Fullscreen Mode?
    {
        DEVMODE dmScreenSettings;								// Device Mode
        memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
        dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
        dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
        dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
        dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
        dmScreenSettings.dmDisplayFrequency	= freq;					// Selected Bits Per Pixel
        dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT|DM_DISPLAYFREQUENCY;

        // Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
        if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
        {
            // If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
            if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
            {
                fullscreen=false;		// Windowed Mode Selected.  Fullscreen = FALSE
            }
            else
            {
                // Pop Up A Message Box Letting User Know The Program Is Closing.
                MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
                return false;									// Return FALSE
            }
        }
    }
#endif

    //so_show();

    //
    // Setup window visuals
    //

    so_print("Main: Setting visual.");
    Fl::visual(FL_DEPTH|FL_DOUBLE|FL_RGB|FL_MULTISAMPLE);

    //
    // Retrieve system colors, if any.
    //

#ifdef __APPLE__
    Fl::scheme("default");
#else
    Fl::scheme("GTK+");
#endif

#ifdef WIN32
    Fl::set_font(FL_HELVETICA, "Segoe UI");
    //Fl::set_font(FL_HELVETICA, "Tahoma");
#endif

#ifdef __APPLE__
    Fl::set_font(FL_HELVETICA, "Lucida Grande");
#endif
    //
    // Determine program path (and remember it...)
    //

    std::string fullPath = argv[0];
    std::string::size_type lastDelim = fullPath.rfind("/");
    std::string progPath = fullPath.substr(0,lastDelim)+"/";

    //
    // Create main window
    //

    so_print("Main: Creating main window.");
    CMainFrame *frame = new CMainFrame();
    //frame->ivfWorkspace->mode(FL_RGB | FL_ALPHA | FL_DEPTH | FL_MULTISAMPLE | FL_DOUBLE );
    frame->setArguments(argc, argv);
    frame->setProgramPath(progPath);
    if (fullscreen)
        frame->show_fullscreen(0,0,width,height);
    else
        frame->show();
    
    
    // Setup leap motion
    Controller controller;
    LeapListener listener;
    controller.addListener(listener);
    listener.setWorkspace(frame->ivfWorkspace);



    //
    // Show a splash screen
    //

    so_print("Main: Creating splash window.");
    CSplashFrame* splash = new CSplashFrame();
    splash->setTimer(true);
    splash->center();
    splash->show();

    //
    // FLTK main loop
    //

    so_print("Main: Entering main loop.");
    
    return Fl::run();

    //
    // Cleanup
    //

    delete frame;
    delete splash;

    so_hide();
}
