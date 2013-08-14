// generated by Fast Light User Interface Designer (fluid) version 1.0300

#include "NodeBCDlg.h"
#include <FL/fl_show_colormap.H>

void CNodeBCDlg::cb_btnApply_i(Fl_Button*, void*) {
  if (m_bc!=NULL)
{
	if (btnXdispl->value()==1)
		m_bc->prescribe(1,edtXdispl->value());
	else
		m_bc->unprescribe(1);

	if (btnYdispl->value()==1)
		m_bc->prescribe(2,edtYdispl->value());
	else
		m_bc->unprescribe(2);

	if (btnZdispl->value()==1)
		m_bc->prescribe(3,edtZdispl->value());
	else
		m_bc->unprescribe(3);

	if (btnXrot->value()==1)
		m_bc->prescribe(4,edtXrot->value());
	else
		m_bc->unprescribe(4);

	if (btnYrot->value()==1)
		m_bc->prescribe(5,edtYrot->value());
	else
		m_bc->unprescribe(5);

	if (btnZrot->value()==1)
		m_bc->prescribe(6,edtZrot->value());
	else
		m_bc->unprescribe(6);

	if (m_workspace!=NULL)
	{
		m_workspace->set_changed();
		m_workspace->redraw();
	}
	uchar r, g, b;
	Fl::get_color(boxColor->color(), r, g, b);
	m_bc->setColor((int) boxColor->color());
	m_bc->setName((char*)edtName->value());
}
m_modalResult = MR_OK;
wndNodeBC->hide();
}
void CNodeBCDlg::cb_btnApply(Fl_Button* o, void* v) {
  ((CNodeBCDlg*)(o->parent()->user_data()))->cb_btnApply_i(o,v);
}

void CNodeBCDlg::cb_btnClose_i(Fl_Button*, void*) {
  m_modalResult = MR_CANCEL;
wndNodeBC->hide();
}
void CNodeBCDlg::cb_btnClose(Fl_Button* o, void* v) {
  ((CNodeBCDlg*)(o->parent()->user_data()))->cb_btnClose_i(o,v);
}

void CNodeBCDlg::cb_btnSetColor_i(Fl_Button*, void*) {
  if (m_bc!=NULL)
{
	m_colorIndex = boxColor->color();
	m_colorIndex = fl_show_colormap(m_colorIndex);
	boxColor->color(m_colorIndex);
	wndNodeBC->redraw();
};
}
void CNodeBCDlg::cb_btnSetColor(Fl_Button* o, void* v) {
  ((CNodeBCDlg*)(o->parent()->parent()->parent()->user_data()))->cb_btnSetColor_i(o,v);
}

void CNodeBCDlg::cb_btnXdispl_i(Fl_Light_Button*, void*) {
  if (btnXdispl->value()==1)
	edtXdispl->activate();
else
	edtXdispl->deactivate();
}
void CNodeBCDlg::cb_btnXdispl(Fl_Light_Button* o, void* v) {
  ((CNodeBCDlg*)(o->parent()->parent()->parent()->user_data()))->cb_btnXdispl_i(o,v);
}

void CNodeBCDlg::cb_btnYdispl_i(Fl_Light_Button*, void*) {
  if (btnYdispl->value()==1)
	edtYdispl->activate();
else
	edtYdispl->deactivate();
}
void CNodeBCDlg::cb_btnYdispl(Fl_Light_Button* o, void* v) {
  ((CNodeBCDlg*)(o->parent()->parent()->parent()->user_data()))->cb_btnYdispl_i(o,v);
}

void CNodeBCDlg::cb_btnZdispl_i(Fl_Light_Button*, void*) {
  if (btnZdispl->value()==1)
	edtZdispl->activate();
else
	edtZdispl->deactivate();
}
void CNodeBCDlg::cb_btnZdispl(Fl_Light_Button* o, void* v) {
  ((CNodeBCDlg*)(o->parent()->parent()->parent()->user_data()))->cb_btnZdispl_i(o,v);
}

void CNodeBCDlg::cb_btnXrot_i(Fl_Light_Button*, void*) {
  if (btnXrot->value()==1)
	edtXrot->activate();
else
	edtXrot->deactivate();
}
void CNodeBCDlg::cb_btnXrot(Fl_Light_Button* o, void* v) {
  ((CNodeBCDlg*)(o->parent()->parent()->parent()->user_data()))->cb_btnXrot_i(o,v);
}

void CNodeBCDlg::cb_btnYrot_i(Fl_Light_Button*, void*) {
  if (btnYrot->value()==1)
	edtYrot->activate();
else
	edtYrot->deactivate();
}
void CNodeBCDlg::cb_btnYrot(Fl_Light_Button* o, void* v) {
  ((CNodeBCDlg*)(o->parent()->parent()->parent()->user_data()))->cb_btnYrot_i(o,v);
}

void CNodeBCDlg::cb_btnZrot_i(Fl_Light_Button*, void*) {
  if (btnZrot->value()==1)
	edtZrot->activate();
else
	edtZrot->deactivate();
}
void CNodeBCDlg::cb_btnZrot(Fl_Light_Button* o, void* v) {
  ((CNodeBCDlg*)(o->parent()->parent()->parent()->user_data()))->cb_btnZrot_i(o,v);
}

CNodeBCDlg::CNodeBCDlg() {
  { wndNodeBC = new Fl_Double_Window(257, 250, "Node BC");
    wndNodeBC->user_data((void*)(this));
    { btnApply = new Fl_Button(119, 223, 67, 23, "Ok");
      btnApply->down_box(FL_PLASTIC_DOWN_BOX);
      btnApply->labelsize(12);
      btnApply->callback((Fl_Callback*)cb_btnApply);
    } // Fl_Button* btnApply
    { btnClose = new Fl_Button(191, 223, 61, 23, "Cancel");
      btnClose->down_box(FL_PLASTIC_DOWN_BOX);
      btnClose->labelsize(12);
      btnClose->callback((Fl_Callback*)cb_btnClose);
    } // Fl_Button* btnClose
    { tbElementBC = new Fl_Tabs(4, 4, 248, 212);
      tbElementBC->box(FL_UP_BOX);
      tbElementBC->labelsize(12);
      { grpGeneral = new Fl_Group(13, 28, 239, 188, "General");
        grpGeneral->labelsize(12);
        { edtName = new Fl_Input(13, 46, 230, 24, "Name");
          edtName->labelsize(12);
          edtName->textsize(12);
          edtName->align(Fl_Align(FL_ALIGN_TOP_LEFT));
        } // Fl_Input* edtName
        { boxColor = new Fl_Box(13, 74, 196, 24, "Color");
          boxColor->box(FL_DOWN_BOX);
          boxColor->color((Fl_Color)93);
          boxColor->labelsize(12);
        } // Fl_Box* boxColor
        { btnSetColor = new Fl_Button(215, 74, 28, 24, "...");
          btnSetColor->down_box(FL_PLASTIC_DOWN_BOX);
          btnSetColor->labelsize(12);
          btnSetColor->callback((Fl_Callback*)cb_btnSetColor);
        } // Fl_Button* btnSetColor
        grpGeneral->end();
      } // Fl_Group* grpGeneral
      { grpData = new Fl_Group(16, 28, 236, 188, "Data");
        grpData->labelsize(12);
        grpData->hide();
        { btnXdispl = new Fl_Light_Button(43, 40, 72, 24, "x-displ.");
          btnXdispl->box(FL_PLASTIC_UP_BOX);
          btnXdispl->down_box(FL_PLASTIC_DOWN_BOX);
          btnXdispl->labelsize(12);
          btnXdispl->callback((Fl_Callback*)cb_btnXdispl);
        } // Fl_Light_Button* btnXdispl
        { btnYdispl = new Fl_Light_Button(43, 68, 72, 24, "y-displ.");
          btnYdispl->box(FL_PLASTIC_UP_BOX);
          btnYdispl->down_box(FL_PLASTIC_DOWN_BOX);
          btnYdispl->labelsize(12);
          btnYdispl->callback((Fl_Callback*)cb_btnYdispl);
        } // Fl_Light_Button* btnYdispl
        { btnZdispl = new Fl_Light_Button(43, 96, 72, 24, "z-displ");
          btnZdispl->box(FL_PLASTIC_UP_BOX);
          btnZdispl->down_box(FL_PLASTIC_DOWN_BOX);
          btnZdispl->labelsize(12);
          btnZdispl->callback((Fl_Callback*)cb_btnZdispl);
        } // Fl_Light_Button* btnZdispl
        { btnXrot = new Fl_Light_Button(43, 124, 72, 24, "x-rot.");
          btnXrot->box(FL_PLASTIC_UP_BOX);
          btnXrot->down_box(FL_PLASTIC_DOWN_BOX);
          btnXrot->labelsize(12);
          btnXrot->callback((Fl_Callback*)cb_btnXrot);
        } // Fl_Light_Button* btnXrot
        { btnYrot = new Fl_Light_Button(43, 152, 72, 24, "y-rot.");
          btnYrot->box(FL_PLASTIC_UP_BOX);
          btnYrot->down_box(FL_PLASTIC_DOWN_BOX);
          btnYrot->labelsize(12);
          btnYrot->callback((Fl_Callback*)cb_btnYrot);
        } // Fl_Light_Button* btnYrot
        { btnZrot = new Fl_Light_Button(43, 180, 72, 24, "z-rot.");
          btnZrot->box(FL_PLASTIC_UP_BOX);
          btnZrot->down_box(FL_PLASTIC_DOWN_BOX);
          btnZrot->labelsize(12);
          btnZrot->callback((Fl_Callback*)cb_btnZrot);
        } // Fl_Light_Button* btnZrot
        { edtZdispl = new Fl_Value_Input(140, 96, 76, 24);
          edtZdispl->box(FL_PLASTIC_DOWN_BOX);
          edtZdispl->labelsize(12);
          edtZdispl->textsize(12);
        } // Fl_Value_Input* edtZdispl
        { edtYdispl = new Fl_Value_Input(140, 68, 76, 24);
          edtYdispl->box(FL_PLASTIC_DOWN_BOX);
          edtYdispl->labelsize(12);
          edtYdispl->textsize(12);
        } // Fl_Value_Input* edtYdispl
        { edtXdispl = new Fl_Value_Input(140, 40, 76, 24);
          edtXdispl->box(FL_PLASTIC_DOWN_BOX);
          edtXdispl->labelsize(12);
          edtXdispl->textsize(12);
        } // Fl_Value_Input* edtXdispl
        { edtXrot = new Fl_Value_Input(140, 124, 76, 24);
          edtXrot->box(FL_PLASTIC_DOWN_BOX);
          edtXrot->labelsize(12);
          edtXrot->textsize(12);
        } // Fl_Value_Input* edtXrot
        { edtYrot = new Fl_Value_Input(140, 152, 76, 24);
          edtYrot->box(FL_PLASTIC_DOWN_BOX);
          edtYrot->labelsize(12);
          edtYrot->textsize(12);
        } // Fl_Value_Input* edtYrot
        { edtZrot = new Fl_Value_Input(140, 180, 76, 24);
          edtZrot->box(FL_PLASTIC_DOWN_BOX);
          edtZrot->labelsize(12);
          edtZrot->textsize(12);
        } // Fl_Value_Input* edtZrot
        grpData->end();
      } // Fl_Group* grpData
      tbElementBC->end();
    } // Fl_Tabs* tbElementBC
    wndNodeBC->set_modal();
    wndNodeBC->end();
  } // Fl_Double_Window* wndNodeBC
  m_bc = NULL;
}

void CNodeBCDlg::show() {
  this->setBC(m_bc);
  wndNodeBC->show();
  while (wndNodeBC->visible()) Fl::wait();
}

void CNodeBCDlg::setBC(CFemBeamNodeBC* bc) {
  m_bc = bc;
  if (m_bc==NULL)
  {
  	btnApply->deactivate();
  	tbElementBC->deactivate();
  }
  else
  {
  	std::string bcName;
  	double value;
  	double ex, ey, ez;
  	bcName = m_bc->getName();
  	btnApply->activate();
  	tbElementBC->activate();
  	edtName->value(bcName.c_str());
  	boxColor->color((Fl_Color) m_bc->getColor());
  
  	if (m_bc->isPrescribed(1))
  	{
  		btnXdispl->value(1);
  		edtXdispl->activate();
  	}
  	else
  	{
  		btnXdispl->value(0);
  		edtXdispl->deactivate();
  	}
  
  	if (m_bc->isPrescribed(2))
  	{
  		btnYdispl->value(1);
  		edtYdispl->activate();
  	}
  	else
  	{
  		btnYdispl->value(0);
  		edtYdispl->deactivate();
  	}
  
  	if (m_bc->isPrescribed(3))
  	{
  		btnZdispl->value(1);
  		edtZdispl->activate();
  	}
  	else
  	{
  		btnZdispl->value(0);
  		edtZdispl->deactivate();
  	}
  
  	if (m_bc->isPrescribed(4))
  	{
  		btnXrot->value(1);
  		edtXrot->activate();
  	}
  	else
  	{
  		btnXrot->value(0);
  		edtXrot->deactivate();
  	}
  
  	if (m_bc->isPrescribed(5))
  	{
  		btnYrot->value(1);
  		edtYrot->activate();
  	}
  	else
  	{
  		btnYrot->value(0);
  		edtYrot->deactivate();
  	}
  
  	if (m_bc->isPrescribed(6))
  	{
  		btnZrot->value(1);
  		edtZrot->activate();
  	}
  	else
  	{
  		btnZrot->value(0);
  		edtZrot->deactivate();
  	}
  
  	edtXdispl->value(m_bc->getPrescribedValue(1));
  	edtYdispl->value(m_bc->getPrescribedValue(2));
  	edtZdispl->value(m_bc->getPrescribedValue(3));
  	edtXrot->value(m_bc->getPrescribedValue(4));
  	edtYrot->value(m_bc->getPrescribedValue(5));
  	edtZrot->value(m_bc->getPrescribedValue(6));
  
  }
}

CFemBeamNodeBC* CNodeBCDlg::getBC() {
  return m_bc;
}

void CNodeBCDlg::setWorkspace(Fl_Widget* widget) {
  m_workspace = widget;
}

void CNodeBCDlg::hide() {
  wndNodeBC->hide();
}

int CNodeBCDlg::getModalResult() {
  return m_modalResult;
}
