#include "fl_ask_win.h"
#ifdef WIN32
#include <FL/Fl.H>
#include <FL/x.H>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

int fl_ask(const char* msg)
{
    static Fl_Window* topWindow = Fl::first_window();

    int result = MessageBox(
        fl_xid(topWindow),
        msg,
        "Message",
        MB_YESNO | MB_ICONQUESTION);

    if (result == IDYES)
        return 1;
    else
        return 0;
}
#endif
