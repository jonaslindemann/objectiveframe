#ifndef _CStatusOutput_h_
#define _CStatusOutput_h_

#include <FL/Fl.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Window.H>
#include <stdarg.h>
#include <string>

#define so_show() CStatusOutput::getInstance()->show();
#define so_print(a) CStatusOutput::getInstance()->print(a);
#define so_hide() CStatusOutput::getInstance()->hide();

class CStatusOutput : public Fl_Window
{
private:
    static CStatusOutput* m_instance;
    Fl_Browser* m_output;

protected:
    CStatusOutput(int x = 0, int y = 0, int w = 410, int h = 160, char* l = 0);

public:
    void show();
    static CStatusOutput* getInstance();
    virtual ~CStatusOutput();

    void print(std::string text);
};

#endif
