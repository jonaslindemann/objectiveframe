/*  Copyright (C) 19998  Craig P. Earls

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
 *HOWTO:
 *
 * Simply #include the header file "StatusBox.hpp" in any source code
 * you want use it from. Three MACROS are defined for simple messages:

 STATUS("This is the message");
 STATUS(somestring);
 WARNING("A warning");
 ERROR("An Error");

 * These macros accept a single character string argument. The only
 * differences are the color fo the text output to the list. If you
 * would like to send more detailed information with formatting you
 * must call status(char *format, ...) directly as follows:

 StatusBox::Instance()->status(
 "I want to print out the following %d %f", anInteger, aFloat);

 or use warning(char *format, ...), or error(char *format, ...);

 * StatusBox is implemented as a singleton, which means the constructor
 * is protected and there is a single static instance. You can only
 * get to the object using the Instance() method, which returns a
 * pointer to the static object. This means all status messages go the
 * same place. This a convenient substitute for a terminal window */

#ifndef StatusBox_hpp
#define StatusBox_hpp
#include <FL/Fl.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Window.H>
#include <stdarg.h>

#define DEBUG 

#ifdef DEBUG
#define SB_STATUS(a...) StatusBox::Instance()->status(a);
#define SB_WARNING(a...) StatusBox::Instance()->warning(a);
#define SB_ERROR(a...) StatusBox::Instance()->error(a);
#else
#define STATUS(a) {};
#define WARNING(a) {}
#define ERROR(a) {};
#endif

class StatusBox : public Fl_Window {
public:
    static StatusBox *Instance();
    void status(char *s, ...);
    void warning(char *s, ...);
    void error(char *s, ...);
    
protected:
    StatusBox(int x=0, int y=0, int w=428, int h=171, char *l="Status");
private:
    Fl_Browser *list;
    Fl_Button *clear;

    inline void cb_clear_i(Fl_Button*, void*);
    static void cb_clear(Fl_Button*, void*);
    Fl_Button *hideb;

    static StatusBox *_Instance;
    
};
#endif
/*
 * Local variables:
 *  compile-command: "make -C .. -k"
 * End:
 */
