/*  Copyright (C) 1998  Craig P. Earls

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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "StatusBox.hpp"

StatusBox *StatusBox::_Instance=0;

StatusBox* StatusBox::Instance()
{
    if (_Instance==0){
        _Instance= new StatusBox();
    };
    
    return _Instance;
}

            
inline void StatusBox::cb_clear_i(Fl_Button*, void*) {
  list->clear();
}

void StatusBox::cb_clear(Fl_Button* o, void* v) {
  ((StatusBox*)(o->parent()->user_data()))->cb_clear_i(o,v);
}

StatusBox::StatusBox(int x=0, int y=0, int w=428, int h=171, char *l="Status"):
        Fl_Window(x, y, w, h, l)
{
    box(FL_UP_BOX);
    user_data((void*)(this));
    {
        Fl_Browser* o = list = new Fl_Browser(10, 10, 410, 120);
        o->color(FL_BLACK);
        Fl_Group::current()->resizable(o);
    }
    { Fl_Button* o = clear = new Fl_Button(10, 135, 410, 30, "&Clear");
      o->callback((Fl_Callback*)cb_clear);
    }
    end();
}


void StatusBox::status(char *format, ...) {
    va_list arg;

    va_start (arg, format);
    
    char *s1, *s2;
    s1=(char *)malloc( 255 * sizeof(char));
    s2=(char *)malloc( 255 * sizeof(char));

    vsprintf(s2, format, arg);
    sprintf(s1, "@B%d@C%d@.%s", FL_BLACK, FL_GREEN, s2);
    list->add(s1);
    show();
    va_end(arg);
    
}

void StatusBox::warning(char *format, ...) {
    va_list arg;

    va_start (arg, format);
    
    char *s1, *s2;
    s1=(char *)malloc( 255 * sizeof(char));
    s2=(char *)malloc( 255 * sizeof(char));

    vsprintf(s2, format, arg);
    sprintf(s1, "@m@B%d@C%d@.%s", FL_BLACK, FL_YELLOW, s2);
    list->add(s1);
    show();
    va_end(arg);
}

void StatusBox::error(char *format, ...) {
    va_list arg;

    va_start (arg, format);
    
    char *s1, *s2;
    s1=(char *)malloc( 255 * sizeof(char));
    s2=(char *)malloc( 255 * sizeof(char));

    vsprintf(s2, format, arg);
    sprintf(s1, "@l@B%d@C%d@.%s", FL_BLACK, FL_RED, s2);
    list->add(s1);
    show();
    va_end(arg);
}

/*
 * Local variables:
 *  compile-command: "make -C .. -k"
 * End:
 */
