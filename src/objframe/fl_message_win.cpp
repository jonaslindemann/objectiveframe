#include "fl_message_win.H"
#ifdef WIN32
#include <FL/Fl.H>
#include <FL/x.H>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

extern "C"
{
    int vsnprintf(char* str, size_t size, const char* fmt, va_list ap);
}

static int innards(const char* fmt, va_list ap)
{
    char buffer[1024];
    if (!strcmp(fmt, "%s"))
    {
        strcpy(buffer, va_arg(ap, const char*));
    }
    else
    {
        vsnprintf(buffer, 1024, fmt, ap);
    }

    static Fl_Window* topWindow = Fl::first_window();

    int result = MessageBox(
        fl_xid(topWindow),
        buffer,
        "Message",
        MB_OK | MB_ICONINFORMATION);

    return true;
}

void fl_message_win(const char* fmt, ...)
{
    va_list ap;

#ifdef WIN32
    MessageBeep(MB_ICONASTERISK);
#endif // WIN32

    va_start(ap, fmt);
    innards(fmt, ap);
}

#endif
