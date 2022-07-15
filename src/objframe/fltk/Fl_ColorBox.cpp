// Fl_Gl_ColorBox.cpp: implementation of the Fl_Gl_ColorBox class.
//
//////////////////////////////////////////////////////////////////////

#include "Fl_ColorBox.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Fl_ColorBox::Fl_ColorBox(int x,int y,int w,int h, const char *l)
    : Fl_Widget(x,y,w,h,l)
{

}

Fl_ColorBox::~Fl_ColorBox()
{

}

int Fl_ColorBox::handle(int event)
{
    return Fl_Widget::handle(event);
    /*
    	switch (event) {
    	default:
    		break;
    	}
    */
}

void Fl_ColorBox::draw()
{
    uchar r, g, b;

    r = (uchar)(m_color[0]*255.0);
    g = (uchar)(m_color[1]*255.0);
    b = (uchar)(m_color[2]*255.0);

    fl_rectf(x(), y(), w(), h(), r, g, b);
}

void Fl_ColorBox::setColor(double red, double green, double blue)
{
    m_color[0] = red;
    m_color[1] = green;
    m_color[2] = blue;
    redraw();
}
