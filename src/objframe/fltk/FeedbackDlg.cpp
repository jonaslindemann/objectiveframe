// generated by Fast Light User Interface Designer (fluid) version 1.0400

#include "FeedbackDlg.h"

CFeedbackDlg::CFeedbackDlg() {
  { wndProcessing = new Fl_Double_Window(406, 73, "Objective frame");
    wndProcessing->user_data((void*)(this));
    { Fl_Box* o = new Fl_Box(0, 0, 406, 73, "Precalculating...");
      o->box(FL_PLASTIC_UP_BOX);
      o->labelfont(2);
      o->labelsize(44);
    } // Fl_Box* o
    wndProcessing->set_modal();
    wndProcessing->clear_border();
    wndProcessing->end();
  } // Fl_Double_Window* wndProcessing
}

CFeedbackDlg::~CFeedbackDlg() {
  delete wndProcessing;
}

void CFeedbackDlg::show() {
  wndProcessing->position(Fl::w()/2-wndProcessing->w()/2,Fl::h()/2-wndProcessing->h()/2);
  wndProcessing->show();
}

void CFeedbackDlg::hide() {
  wndProcessing->hide();
}
