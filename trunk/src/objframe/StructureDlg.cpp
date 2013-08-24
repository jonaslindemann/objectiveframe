// generated by Fast Light User Interface Designer (fluid) version 1.0302

#include "StructureDlg.h"
#include <iostream.h>

void CStructureDlg::cb_btnOk_i(Fl_Button*, void*) {
  m_modalResult = MR_OK;
wndWorkspace->hide();
}
void CStructureDlg::cb_btnOk(Fl_Button* o, void* v) {
  ((CStructureDlg*)(o->parent()->user_data()))->cb_btnOk_i(o,v);
}

void CStructureDlg::cb_btnCancel_i(Fl_Button*, void*) {
  m_modalResult = MR_CANCEL;
wndWorkspace->hide();
}
void CStructureDlg::cb_btnCancel(Fl_Button* o, void* v) {
  ((CStructureDlg*)(o->parent()->user_data()))->cb_btnCancel_i(o,v);
}

CStructureDlg::CStructureDlg() {
  { wndWorkspace = new Fl_Double_Window(312, 231, "Create structure");
    wndWorkspace->user_data((void*)(this));
    { btnOk = new Fl_Button(239, 5, 67, 25, "OK");
      btnOk->box(FL_PLASTIC_UP_BOX);
      btnOk->down_box(FL_PLASTIC_DOWN_BOX);
      btnOk->labelsize(12);
      btnOk->callback((Fl_Callback*)cb_btnOk);
    } // Fl_Button* btnOk
    { btnCancel = new Fl_Button(239, 36, 67, 25, "Cancel");
      btnCancel->box(FL_PLASTIC_UP_BOX);
      btnCancel->down_box(FL_PLASTIC_DOWN_BOX);
      btnCancel->labelsize(12);
      btnCancel->callback((Fl_Callback*)cb_btnCancel);
    } // Fl_Button* btnCancel
    { grpWorkspace = new Fl_Group(4, 5, 228, 222);
      grpWorkspace->box(FL_PLASTIC_DOWN_FRAME);
      { sldRows = new Fl_Value_Slider(16, 28, 201, 20, "Number of rows ");
        sldRows->type(1);
        sldRows->box(FL_PLASTIC_DOWN_BOX);
        sldRows->labelsize(12);
        sldRows->minimum(2);
        sldRows->maximum(30);
        sldRows->step(1);
        sldRows->value(4);
        sldRows->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Value_Slider* sldRows
      { sldColumns = new Fl_Value_Slider(16, 66, 201, 20, "Number of columns");
        sldColumns->type(1);
        sldColumns->box(FL_PLASTIC_DOWN_BOX);
        sldColumns->labelsize(12);
        sldColumns->minimum(2);
        sldColumns->maximum(30);
        sldColumns->step(1);
        sldColumns->value(4);
        sldColumns->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Value_Slider* sldColumns
      { sldStacks = new Fl_Value_Slider(16, 104, 201, 20, "Number of stacks");
        sldStacks->type(1);
        sldStacks->box(FL_PLASTIC_DOWN_BOX);
        sldStacks->labelsize(12);
        sldStacks->minimum(2);
        sldStacks->maximum(30);
        sldStacks->step(1);
        sldStacks->value(4);
        sldStacks->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      } // Fl_Value_Slider* sldStacks
      { edtZspacing = new Fl_Value_Input(112, 139, 105, 22, "Z spacing (units)");
        edtZspacing->box(FL_PLASTIC_DOWN_BOX);
        edtZspacing->labelsize(12);
        edtZspacing->minimum(0.1);
        edtZspacing->maximum(1e+300);
        edtZspacing->value(2);
      } // Fl_Value_Input* edtZspacing
      { edtXspacing = new Fl_Value_Input(112, 166, 105, 22, "X spacing (units)");
        edtXspacing->box(FL_PLASTIC_DOWN_BOX);
        edtXspacing->labelsize(12);
        edtXspacing->minimum(0.1);
        edtXspacing->maximum(1e+300);
        edtXspacing->value(2);
      } // Fl_Value_Input* edtXspacing
      { edtYspacing = new Fl_Value_Input(112, 193, 105, 22, "Y spacing (units)");
        edtYspacing->box(FL_PLASTIC_DOWN_BOX);
        edtYspacing->labelsize(12);
        edtYspacing->minimum(0.1);
        edtYspacing->maximum(1e+300);
        edtYspacing->value(2);
      } // Fl_Value_Input* edtYspacing
      grpWorkspace->end();
    } // Fl_Group* grpWorkspace
    wndWorkspace->set_modal();
    wndWorkspace->end();
  } // Fl_Double_Window* wndWorkspace
}

void CStructureDlg::show() {
  wndWorkspace->show();
  while (wndWorkspace->visible()) Fl::wait();
}

int CStructureDlg::getModalResult() {
  return m_modalResult;
}

CStructureDlg::~CStructureDlg() {
  delete wndWorkspace;
}

void CStructureDlg::getSize(int &rows, int &cols, int &stacks) {
  m_size[0] = sldRows->value();
  m_size[1] = sldColumns->value();
  m_size[2] = sldStacks->value();
  rows = m_size[0];
  cols = m_size[1];
  stacks = m_size[2];
}

void CStructureDlg::getSpacing(double &xSpacing, double &ySpacing, double &zSpacing) {
  m_spacing[0] = edtXspacing->value();
  m_spacing[1] = edtYspacing->value();
  m_spacing[2] = edtZspacing->value();
  xSpacing = m_spacing[0];
  ySpacing = m_spacing[1];
  zSpacing = m_spacing[2];
}
