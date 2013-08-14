#ifndef Fl_HoverButton_H
#define Fl_HoverButton_H

#include <FL/Fl_Button.H>

/**
 * Hover button class
 *
 * Specialized FLTK Fl_Button class. Implementing
 * a hovering button. The button is flat when mouse 
 * is not over it. When mouse is over the button adds
 * an up frame.
 */
class Fl_HoverButton : public Fl_Button {
private:
	bool m_moreButton;
public:
	void setMoreButton(bool flag);
	/** Fl_HoverButton class constructor. */
	Fl_HoverButton(int,int,int,int,const char * = 0);

	/** Fl_HoverButton class constructor. */
	virtual ~Fl_HoverButton();
protected:
	/** Overridden FLTK handle method. */
	int handle(int event);

	/** Overridden FLTK draw method. */
	void draw();
};

#endif 
