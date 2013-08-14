#include "StatusOutput.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

CStatusOutput* CStatusOutput::m_instance=0;

CStatusOutput::CStatusOutput(int x, int y, int w, int h, char *l):
        Fl_Window(x, y, w, h, l)
{
    box(FL_FLAT_BOX);
    user_data((void*)(this));
	m_output = new Fl_Browser(5, 5, 400,150);
	m_output->color(FL_BLACK);
	Fl_Group::current()->resizable(m_output);
    end();
}

CStatusOutput::~CStatusOutput()
{

}

CStatusOutput* CStatusOutput::getInstance()
{
    if (m_instance==0)
        m_instance = new CStatusOutput();
    
    return m_instance;
}


void CStatusOutput::print(char* txt)
{
	char* buffer = new char[255];
    sprintf(buffer, "@s@f@B%d@C%d@.%s", FL_BLACK, FL_GREEN, txt);
    m_output->add(buffer);
	m_output->bottomline(m_output->size());
	delete [] buffer;
}

void CStatusOutput::show()
{
	Fl_Window::show();
}
