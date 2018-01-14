// generated by Fast Light User Interface Designer (fluid) version 1.0400

#ifndef SplashFrame_h
#define SplashFrame_h
#include <FL/Fl.H>
void cbHide(void* w);
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>

class CSplashFrame {
  bool m_timer; 
public:
  CSplashFrame();
  Fl_Double_Window *wndSplash;
  Fl_Box *boxImage;
  Fl_Button *btnClose;
private:
  inline void cb_btnClose_i(Fl_Button*, void*);
  static void cb_btnClose(Fl_Button*, void*);
public:
  void show();
  void center();
  void setTimer(bool flag);
  void hide();
};
#endif
