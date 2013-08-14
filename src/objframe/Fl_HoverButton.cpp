// Fl_HoverButton.cpp: implementation of the Fl_HoverButton class.
//
//////////////////////////////////////////////////////////////////////

#include "Fl_HoverButton.h"
#include <FL/Fl.H>
#include <FL/fl_draw.H>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Fl_HoverButton::Fl_HoverButton(int x,int y,int w,int h, const char *l)
: Fl_Button(x,y,w,h,l)
{
	m_moreButton = false;
}

Fl_HoverButton::~Fl_HoverButton()
{

}

int Fl_HoverButton::handle(int event)
{
	switch (event) {
	case FL_ENTER:

		redraw();
		return 1;
		break;
	case FL_LEAVE:

		redraw();
		return 1;
		break;
	default:
		return Fl_Button::handle(event);
		break;
	}
}

void Fl_HoverButton::draw()
{
	if (type() == FL_HIDDEN_BUTTON || box() == FL_NO_BOX) return;
	Fl_Color col = value() ? selection_color() : color();
	if (col == FL_GRAY && Fl::belowmouse()==this) col = FL_LIGHT1;
	
	if (value())
	{
		// Pressed

		if (down_box())
			draw_box(down_box(), col);
		else
			draw_box(fl_down(box()), col);
	}
	else
	{
		// Unpressed

		if (Fl::belowmouse()==this)
			draw_box(box(), col);
		else
			draw_box(FL_FLAT_BOX, col);
	}
	
	//draw_box(value() ? (down_box()?down_box():down(box())) : box(), col);
	draw_label();

	if (m_moreButton)
	{
		fl_color(FL_DARK3); 
		
		int xx = x()+w()-7;
		int yy = y()+w()-7;
		
		fl_polygon(xx,yy,xx+5,yy+2,xx,yy+4);
	}
}

void Fl_HoverButton::setMoreButton(bool flag)
{
	m_moreButton = true;
	redraw();
}
