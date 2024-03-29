// generated by Fast Light User Interface Designer (fluid) version 1.0400

#include "NodeLoadDlg.h"
#include <FL/fl_show_colormap.H>

void NodeLoadDlg::cb_btnApply_i(Fl_Button*, void*) {
  if (m_load!=NULL)
{
	if (m_workspace!=NULL)
	{
		m_workspace->set_changed();
		m_workspace->redraw();
	}
	uchar r, g, b;
	Fl::get_color(boxColor->color(), r, g, b);
	m_load->setValue(edtValue->value());
	m_load->setName((char*)edtName->value());
	if (rbXDirection->value()>0)
		m_load->setDirection(1.0, 0.0, 0.0);
	else if (rbYDirection->value()>0)
		m_load->setDirection(0.0, 1.0, 0.0);
	else if (rbZDirection->value()>0)
		m_load->setDirection(0.0, 0.0, 1.0);
	m_load->setColor((int) boxColor->color());
}
m_modalResult = MR_OK;
wndNodeLoad->hide();
}
void NodeLoadDlg::cb_btnApply(Fl_Button* o, void* v) {
  ((NodeLoadDlg*)(o->parent()->user_data()))->cb_btnApply_i(o,v);
}

void NodeLoadDlg::cb_btnClose_i(Fl_Button*, void*) {
  m_modalResult = MR_CANCEL;
wndNodeLoad->hide();
}
void NodeLoadDlg::cb_btnClose(Fl_Button* o, void* v) {
  ((NodeLoadDlg*)(o->parent()->user_data()))->cb_btnClose_i(o,v);
}

void NodeLoadDlg::cb_btnSetColor_i(Fl_Button*, void*) {
  if (m_load!=NULL)
{
	m_colorIndex = boxColor->color();
	m_colorIndex = fl_show_colormap(m_colorIndex);
	boxColor->color(m_colorIndex);
	wndNodeLoad->redraw();
};
}
void NodeLoadDlg::cb_btnSetColor(Fl_Button* o, void* v) {
  ((NodeLoadDlg*)(o->parent()->parent()->parent()->user_data()))->cb_btnSetColor_i(o,v);
}

NodeLoadDlg::NodeLoadDlg() {
  { wndNodeLoad = new Fl_Double_Window(253, 220, "Node load");
    wndNodeLoad->user_data((void*)(this));
    { btnApply = new Fl_Button(115, 190, 67, 25, "Ok");
      btnApply->down_box(FL_PLASTIC_DOWN_BOX);
      btnApply->labelsize(12);
      btnApply->callback((Fl_Callback*)cb_btnApply);
    } // Fl_Button* btnApply
    { btnClose = new Fl_Button(187, 190, 61, 25, "Cancel");
      btnClose->down_box(FL_PLASTIC_DOWN_BOX);
      btnClose->labelsize(12);
      btnClose->callback((Fl_Callback*)cb_btnClose);
    } // Fl_Button* btnClose
    { tbElementLoad = new Fl_Tabs(4, 4, 244, 179);
      tbElementLoad->box(FL_UP_BOX);
      tbElementLoad->labelsize(12);
      { grpGeneral = new Fl_Group(11, 28, 237, 140, "General");
        grpGeneral->labelsize(12);
        { edtName = new Fl_Input(12, 48, 227, 24, "Name");
          edtName->labelsize(12);
          edtName->textsize(12);
          edtName->align(Fl_Align(FL_ALIGN_TOP_LEFT));
        } // Fl_Input* edtName
        { boxColor = new Fl_Box(12, 80, 195, 24, "Color");
          boxColor->box(FL_DOWN_BOX);
          boxColor->color((Fl_Color)93);
          boxColor->labelsize(12);
        } // Fl_Box* boxColor
        { btnSetColor = new Fl_Button(211, 80, 28, 24, "...");
          btnSetColor->down_box(FL_PLASTIC_DOWN_BOX);
          btnSetColor->labelsize(12);
          btnSetColor->callback((Fl_Callback*)cb_btnSetColor);
        } // Fl_Button* btnSetColor
        grpGeneral->end();
      } // Fl_Group* grpGeneral
      { grpData = new Fl_Group(12, 28, 236, 155, "Data");
        grpData->labelsize(12);
        grpData->hide();
        { edtValue = new Fl_Value_Input(72, 52, 100, 24, "Load value");
          edtValue->box(FL_PLASTIC_DOWN_BOX);
          edtValue->labelsize(12);
          edtValue->textsize(12);
          edtValue->align(Fl_Align(FL_ALIGN_TOP_LEFT));
        } // Fl_Value_Input* edtValue
        { grpLoadDirection = new Fl_Group(72, 96, 116, 76, "Load direction");
          grpLoadDirection->box(FL_PLASTIC_DOWN_FRAME);
          grpLoadDirection->labelsize(12);
          grpLoadDirection->align(Fl_Align(FL_ALIGN_TOP_LEFT));
          { rbXDirection = new Fl_Round_Button(80, 104, 68, 24, "X");
            rbXDirection->type(102);
            rbXDirection->down_box(FL_PLASTIC_DOWN_BOX);
            rbXDirection->labelsize(12);
          } // Fl_Round_Button* rbXDirection
          { rbYDirection = new Fl_Round_Button(80, 124, 72, 24, "Y");
            rbYDirection->type(102);
            rbYDirection->down_box(FL_PLASTIC_DOWN_BOX);
            rbYDirection->labelsize(12);
          } // Fl_Round_Button* rbYDirection
          { rbZDirection = new Fl_Round_Button(80, 144, 72, 24, "Z");
            rbZDirection->type(102);
            rbZDirection->down_box(FL_PLASTIC_DOWN_BOX);
            rbZDirection->labelsize(12);
          } // Fl_Round_Button* rbZDirection
          grpLoadDirection->end();
        } // Fl_Group* grpLoadDirection
        grpData->end();
      } // Fl_Group* grpData
      tbElementLoad->end();
    } // Fl_Tabs* tbElementLoad
    wndNodeLoad->set_modal();
    wndNodeLoad->end();
  } // Fl_Double_Window* wndNodeLoad
  m_load = NULL;
}

void NodeLoadDlg::show() {
  this->setLoad(m_load);
  wndNodeLoad->show();
  while (wndNodeLoad->visible()) Fl::wait();
}

void NodeLoadDlg::setLoad(CFemBeamNodeLoad* load) {
  m_load = load;
  if (m_load==NULL)
  {
  	btnApply->deactivate();
  	tbElementLoad->deactivate();
  }
  else
  {
  	std::string loadName;
  	double value;
  	double ex, ey, ez;
  	loadName = m_load->getName();
  	m_load->getDirection(ex, ey, ez);
  	value = m_load->getValue();
  	btnApply->activate();
  	tbElementLoad->activate();
  	edtName->value(loadName.c_str());
  	edtValue->value(value);
  	if (ex>0)
  		rbXDirection->value(1);
  	else if (ey>0)
  		rbYDirection->value(1);
  	else if (ez>0)
  		rbZDirection->value(1);
  	boxColor->color((Fl_Color) m_load->getColor());
  }
}

CFemBeamNodeLoad* NodeLoadDlg::getLoad() {
  return m_load;
}

void NodeLoadDlg::setWorkspace(Fl_Widget* widget) {
  m_workspace = widget;
}

void NodeLoadDlg::hide() {
  wndNodeLoad->hide();
}

int NodeLoadDlg::getModalResult() {
  return m_modalResult;
}
