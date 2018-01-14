// generated by Fast Light User Interface Designer (fluid) version 1.0400

#include "ScalefactorDlg.h"
#include "IvfFemWidget.h"

void CScalefactorDlg::cb_btnClose_i(Fl_Button*, void*) {
  wndWorkspace->hide();
}
void CScalefactorDlg::cb_btnClose(Fl_Button* o, void* v) {
  ((CScalefactorDlg*)(o->parent()->user_data()))->cb_btnClose_i(o,v);
}

void CScalefactorDlg::cb_sldScalefactor_i(Fl_Value_Slider*, void*) {
  if (m_femWidget!=NULL)
{
	CIvfFemWidget* femWidget = (CIvfFemWidget*)m_femWidget;
	femWidget->setScalefactor(sldScalefactor->value());
};
}
void CScalefactorDlg::cb_sldScalefactor(Fl_Value_Slider* o, void* v) {
  ((CScalefactorDlg*)(o->parent()->parent()->user_data()))->cb_sldScalefactor_i(o,v);
}

CScalefactorDlg::CScalefactorDlg() {
  { wndWorkspace = new Fl_Double_Window(310, 100, "Workspace");
    wndWorkspace->user_data((void*)(this));
    { btnClose = new Fl_Button(236, 6, 67, 25, "Close");
      btnClose->box(FL_PLASTIC_UP_BOX);
      btnClose->down_box(FL_PLASTIC_DOWN_BOX);
      btnClose->callback((Fl_Callback*)cb_btnClose);
    } // Fl_Button* btnClose
    { grpWorkspace = new Fl_Group(6, 5, 223, 89);
      grpWorkspace->box(FL_PLASTIC_DOWN_FRAME);
      { sldScalefactor = new Fl_Value_Slider(18, 41, 201, 20, "Scalefactor");
        sldScalefactor->type(5);
        sldScalefactor->box(FL_PLASTIC_DOWN_BOX);
        sldScalefactor->minimum(1);
        sldScalefactor->maximum(1e+06);
        sldScalefactor->step(0);
        sldScalefactor->value(1);
        sldScalefactor->callback((Fl_Callback*)cb_sldScalefactor);
        sldScalefactor->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Value_Slider* sldScalefactor
      grpWorkspace->end();
    } // Fl_Group* grpWorkspace
    wndWorkspace->set_non_modal();
    wndWorkspace->end();
  } // Fl_Double_Window* wndWorkspace
  m_femWidget = NULL;
}

CScalefactorDlg::~CScalefactorDlg() {
  delete wndWorkspace;
}

void CScalefactorDlg::show() {
  if (m_femWidget!=NULL)
  {
  	CIvfFemWidget* femWidget = (CIvfFemWidget*) m_femWidget;
  	sldScalefactor->value(femWidget->getScalefactor());
  }
  wndWorkspace->show();
}

void CScalefactorDlg::setFemWidget(void* femWidget) {
  m_femWidget = femWidget;
}

void CScalefactorDlg::setScaling(double maxValue) {
  m_maxValue = maxValue;
  sldScalefactor->maximum(maxValue*10);
  sldScalefactor->minimum(maxValue*0.1);
  sldScalefactor->step((maxValue*10-maxValue*0.1)/100.0);
}
