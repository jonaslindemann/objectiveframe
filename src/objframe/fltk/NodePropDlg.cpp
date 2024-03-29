// generated by Fast Light User Interface Designer (fluid) version 1.0400

#include "NodePropDlg.h"

void NodePropDlg::cb_btnApply_i(Fl_Button*, void*) {
  if (m_node!=NULL)
{
	m_node->setPosition(
		edtXCoord->value(),
		edtYCoord->value(),
		edtZCoord->value());
	if (m_workspace!=NULL)
	{
		m_workspace->set_changed();
		m_workspace->redraw();
	}
};
}
void NodePropDlg::cb_btnApply(Fl_Button* o, void* v) {
  ((NodePropDlg*)(o->parent()->user_data()))->cb_btnApply_i(o,v);
}

void NodePropDlg::cb_btnClose_i(Fl_Button*, void*) {
  wndNodeProp->hide();
}
void NodePropDlg::cb_btnClose(Fl_Button* o, void* v) {
  ((NodePropDlg*)(o->parent()->user_data()))->cb_btnClose_i(o,v);
}

NodePropDlg::NodePropDlg() {
  { wndNodeProp = new Fl_Double_Window(184, 268, "Node properties");
    wndNodeProp->color(FL_FOREGROUND_COLOR);
    wndNodeProp->user_data((void*)(this));
    { btnApply = new Fl_Button(108, 235, 67, 25, "Apply");
      btnApply->box(FL_FLAT_BOX);
      btnApply->down_box(FL_FLAT_BOX);
      btnApply->color((Fl_Color)41);
      btnApply->labelsize(12);
      btnApply->callback((Fl_Callback*)cb_btnApply);
    } // Fl_Button* btnApply
    { grpWorkspace = new Fl_Group(8, 19, 169, 96, "Coordinates");
      grpWorkspace->box(FL_FLAT_BOX);
      grpWorkspace->color((Fl_Color)36);
      grpWorkspace->labelsize(12);
      grpWorkspace->labelcolor(FL_BACKGROUND2_COLOR);
      grpWorkspace->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      { edtXCoord = new Fl_Value_Input(94, 26, 74, 24, "X-Coordinate");
        edtXCoord->box(FL_FLAT_BOX);
        edtXCoord->color((Fl_Color)41);
        edtXCoord->labelsize(12);
        edtXCoord->labelcolor(FL_BACKGROUND2_COLOR);
        edtXCoord->minimum(-1e+300);
        edtXCoord->maximum(1e+300);
        edtXCoord->textsize(12);
      } // Fl_Value_Input* edtXCoord
      { edtZCoord = new Fl_Value_Input(94, 82, 74, 25, "Z-Coordinate");
        edtZCoord->box(FL_FLAT_BOX);
        edtZCoord->color((Fl_Color)41);
        edtZCoord->labelsize(12);
        edtZCoord->labelcolor(FL_BACKGROUND2_COLOR);
        edtZCoord->textsize(12);
      } // Fl_Value_Input* edtZCoord
      { edtYCoord = new Fl_Value_Input(94, 54, 74, 25, "Y-Coordinate");
        edtYCoord->box(FL_FLAT_BOX);
        edtYCoord->color((Fl_Color)41);
        edtYCoord->labelsize(12);
        edtYCoord->labelcolor(FL_BACKGROUND2_COLOR);
        edtYCoord->textsize(12);
      } // Fl_Value_Input* edtYCoord
      grpWorkspace->end();
    } // Fl_Group* grpWorkspace
    { btnClose = new Fl_Button(40, 235, 61, 25, "Close");
      btnClose->box(FL_FLAT_BOX);
      btnClose->down_box(FL_FLAT_BOX);
      btnClose->color((Fl_Color)41);
      btnClose->labelsize(12);
      btnClose->callback((Fl_Callback*)cb_btnClose);
    } // Fl_Button* btnClose
    { grpDisplacements = new Fl_Group(9, 136, 168, 93, "Computed displacements");
      grpDisplacements->box(FL_FLAT_BOX);
      grpDisplacements->color((Fl_Color)36);
      grpDisplacements->labelsize(12);
      grpDisplacements->labelcolor(FL_BACKGROUND2_COLOR);
      grpDisplacements->align(Fl_Align(FL_ALIGN_TOP_LEFT));
      { edtXDispl = new Fl_Value_Input(91, 142, 78, 25, "X-Displ.");
        edtXDispl->box(FL_FLAT_BOX);
        edtXDispl->color((Fl_Color)41);
        edtXDispl->labelsize(12);
        edtXDispl->labelcolor(FL_BACKGROUND2_COLOR);
        edtXDispl->minimum(-1e+300);
        edtXDispl->maximum(1e+300);
        edtXDispl->textsize(12);
      } // Fl_Value_Input* edtXDispl
      { edtYDispl = new Fl_Value_Input(91, 170, 78, 25, "Y-Displ.");
        edtYDispl->box(FL_FLAT_BOX);
        edtYDispl->color((Fl_Color)41);
        edtYDispl->labelsize(12);
        edtYDispl->labelcolor(FL_BACKGROUND2_COLOR);
        edtYDispl->minimum(-1e+300);
        edtYDispl->maximum(1e+300);
        edtYDispl->textsize(12);
      } // Fl_Value_Input* edtYDispl
      { edtZDispl = new Fl_Value_Input(91, 198, 78, 25, "Z-Displ.");
        edtZDispl->box(FL_FLAT_BOX);
        edtZDispl->color((Fl_Color)41);
        edtZDispl->labelsize(12);
        edtZDispl->labelcolor(FL_BACKGROUND2_COLOR);
        edtZDispl->minimum(-1e+300);
        edtZDispl->maximum(1e+300);
        edtZDispl->textsize(12);
      } // Fl_Value_Input* edtZDispl
      grpDisplacements->end();
    } // Fl_Group* grpDisplacements
    wndNodeProp->set_non_modal();
    wndNodeProp->end();
  } // Fl_Double_Window* wndNodeProp
  m_node = NULL;
}

void NodePropDlg::show() {
  this->setNode(m_node);
  wndNodeProp->show();
}

void NodePropDlg::setNode(VisFemNode* node) {
  m_node = node;
  if (m_node==NULL)
  {
  	edtXCoord->deactivate();
  	edtYCoord->deactivate();
  	edtZCoord->deactivate();
  	edtXDispl->deactivate();
  	edtYDispl->deactivate();
  	edtZDispl->deactivate();
  	btnApply->deactivate();
  	edtXCoord->value(0);
  	edtYCoord->value(0);
  	edtZCoord->value(0);
  	edtXDispl->value(0);
  	edtYDispl->value(0);
  	edtZDispl->value(0);
  }
  else
  {
  	edtXCoord->activate();
  	edtYCoord->activate();
  	edtZCoord->activate();
  	edtXDispl->activate();
  	edtYDispl->activate();
  	edtZDispl->activate();
  	btnApply->activate();
  	double x, y, z;
  	double dx, dy, dz;
  	m_node->getPosition(x, y, z);
  	dx = m_node->getFemNode()->getValue(0);
  	dy = m_node->getFemNode()->getValue(1);
  	dz = m_node->getFemNode()->getValue(2);
  	edtXCoord->value(x);
  	edtYCoord->value(y);
  	edtZCoord->value(z);
  	edtXDispl->value(dx);
  	edtYDispl->value(dy);
  	edtZDispl->value(dz);
  }
}

VisFemNode* NodePropDlg::getNode() {
  return m_node;
}

void NodePropDlg::setWorkspace(Fl_Widget* widget) {
  m_workspace = widget;
}

void NodePropDlg::hide() {
  wndNodeProp->hide();
}
