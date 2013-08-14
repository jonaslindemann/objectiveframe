#ifndef _Fl_Gl_ColorBox_h_
#define _Fl_Gl_ColorBox_h_

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/gl.h>
#include <GL/glu.h>
#include <FL/fl_draw.H>

class Fl_ColorBox : public Fl_Widget {
private:
	double m_color[3];
public:
	Fl_ColorBox(int,int,int,int,const char * = 0);
	/** Fl_HoverButton class constructor. */
	virtual ~Fl_ColorBox();

	void setColor(double red, double green, double blue);

protected:
	/** Overridden FLTK handle method. */
	int handle(int event);

	/** Overridden FLTK draw method. */
	void draw();
};
#endif 
