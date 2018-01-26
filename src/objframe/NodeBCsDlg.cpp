// generated by Fast Light User Interface Designer (fluid) version 1.0400

#include "NodeBCsDlg.h"
#include "IvfFemWidget.h"
#include "NodeBCDlg.h"

void CNodeBCsDlg::cb_lbBCSet_i(Fl_Browser*, void*) {
  if (m_bcSet!=NULL)
{
	if (lbBCSet->value()>0)
	{
		m_currentBC = (CFemBeamNodeBC*)
			m_bcSet->getBC(lbBCSet->value()-1);
	}
	else m_currentBC = NULL;
	if (m_femWidget!=NULL)
	{
		CIvfFemWidget* femWidget = (CIvfFemWidget*) m_femWidget;
		femWidget->setCurrentNodeBC(m_currentBC);
	}
};
}
void CNodeBCsDlg::cb_lbBCSet(Fl_Browser* o, void* v) {
  ((CNodeBCsDlg*)(o->parent()->user_data()))->cb_lbBCSet_i(o,v);
}

void CNodeBCsDlg::cb_btnAddBC_i(Fl_HoverButton*, void*) {
  if (m_bcSet!=NULL)
{
	char loadName[255];
	const char* result;
	result = fl_input("BC name");
	if (result!=NULL)
	{
		strcpy(loadName, result);
		if (loadName!=NULL)
		{
			CFemBeamNodeBC* bc = new CFemBeamNodeBC();
			bc->setName((char*)loadName);
			m_bcSet->addBC(bc);
			if (m_femWidget!=NULL)
			{
				CIvfFemWidget* femWidget = (CIvfFemWidget*)m_femWidget;
				femWidget->addNodeBC(bc);
				this->fillListBox();
			}
		}
	}
};
}
void CNodeBCsDlg::cb_btnAddBC(Fl_HoverButton* o, void* v) {
  ((CNodeBCsDlg*)(o->parent()->user_data()))->cb_btnAddBC_i(o,v);
}

#include <FL/Fl_Pixmap.H>
static const char *idata_tlAddBC[] = {
"    18    18        5            1",
"` c #000081",
". c #008100",
"# c #00ff00",
"a c none",
"b c #ffffff",
"aaaaaaaaaaaaaaaaaa",
"aaaaaaaaaaaaa``aaa",
"aaaaaaaaaaaaa``aaa",
"aaaaaaaaaaa``````a",
"aaaaaaaaaaa``````a",
"aaaaaaaaaaaaa``aaa",
"aaaaaaaaabaab``aaa",
"aaaaaaaab#ab#aaaaa",
"aaaaaaab##b##aaaaa",
"aaaaaaaa..a..aaaaa",
"aaaaaaaaa.aa.aaaaa",
"aaaabaaaaaaaaaaaaa",
"aaab#.aaaaaaaaaaaa",
"aab##..aaaaaaaaaaa",
"aaaabaaaaaaaaaaaaa",
"aaab#.aaaaaaaaaaaa",
"aab##..aaaaaaaaaaa",
"aaaaaaaaaaaaaaaaaa"
};
static Fl_Image *image_tlAddBC() {
  static Fl_Image *image = new Fl_Pixmap(idata_tlAddBC);
  return image;
}

void CNodeBCsDlg::cb_btnRemoveBC_i(Fl_HoverButton*, void*) {
  if (m_currentBC!=NULL)
{
	if (m_femWidget!=NULL)
	{
		CIvfFemWidget* femWidget = (CIvfFemWidget*) m_femWidget;
		femWidget->deleteNodeBC(m_currentBC);
		this->fillListBox();
	}
};
}
void CNodeBCsDlg::cb_btnRemoveBC(Fl_HoverButton* o, void* v) {
  ((CNodeBCsDlg*)(o->parent()->user_data()))->cb_btnRemoveBC_i(o,v);
}

static const char *idata_tlDelBC[] = {
"    18    18        5            1",
"` c #000081",
". c #008100",
"# c #00ff00",
"a c none",
"b c #ffffff",
"aaaaaaaaaaaaaaaaaa",
"aaaaaaaaaaaaaaaaaa",
"aaaaaaaaaaaaaaaaaa",
"aaaaaaaaaaa``````a",
"aaaaaaaaaaa``````a",
"aaaaaaaaaaaaaaaaaa",
"aaaaaaaaabaabaaaaa",
"aaaaaaaab#ab#aaaaa",
"aaaaaaab##b##aaaaa",
"aaaaaaaa..a..aaaaa",
"aaaaaaaaa.aa.aaaaa",
"aaaabaaaaaaaaaaaaa",
"aaab#.aaaaaaaaaaaa",
"aab##..aaaaaaaaaaa",
"aaaabaaaaaaaaaaaaa",
"aaab#.aaaaaaaaaaaa",
"aab##..aaaaaaaaaaa",
"aaaaaaaaaaaaaaaaaa"
};
static Fl_Image *image_tlDelBC() {
  static Fl_Image *image = new Fl_Pixmap(idata_tlDelBC);
  return image;
}

void CNodeBCsDlg::cb_btnEditBC_i(Fl_HoverButton*, void*) {
  if (this->getCurrentBC()!=NULL)
{
	CNodeBCDlg* dlg = new CNodeBCDlg();
	dlg->setBC(this->getCurrentBC());
	dlg->setWorkspace((Fl_Widget*)m_femWidget);
	dlg->show();
	if (dlg->getModalResult()==MR_OK)
	{
		//ivfWorkspace->setWorkspace(dlg->getWorkspaceSize());
	}
	delete dlg;
	this->fillListBox();
};
}
void CNodeBCsDlg::cb_btnEditBC(Fl_HoverButton* o, void* v) {
  ((CNodeBCsDlg*)(o->parent()->user_data()))->cb_btnEditBC_i(o,v);
}

static const char *idata_tlInspBC[] = {
"    18    18        6            1",
"` c #000000",
". c #000081",
"# c #008100",
"a c #00ff00",
"b c none",
"c c #ffffff",
"bbbbbbbbbbbbbbbbbb",
"bbbbbbbbcbbcbbbbbb",
"bbbbbbbcabcabbbbbb",
"bbbbbbcaacaabbbbbb",
"bbbbbbb##b##bbbbbb",
"bbbbbbbb#bb#bbbbbb",
"bbbcbbbbbbbbbbbbbb",
"bbca#bbbbbbbbbbbbb",
"bcaa##bbbbbbbbbbb.",
"bbbcbbbbb``````b..",
"bbca#bbb`cccccc`..",
"bcaa##b`c`cccccc..",
"bbbbbb`c`c`ccccc..",
"bbbbb`c`c`c`ccc`..",
"bbbb`c`b`c`c```b..",
"bbbbb`bbb`c`bbbbb.",
"bbbbbbbbbb`bbbbbbb",
"bbbbbbbbbbbbbbbbbb"
};
static Fl_Image *image_tlInspBC() {
  static Fl_Image *image = new Fl_Pixmap(idata_tlInspBC);
  return image;
}

void CNodeBCsDlg::cb_btnAssignBC_i(Fl_HoverButton*, void*) {
  if (this->getCurrentBC()!=NULL)
{
	if (m_femWidget!=NULL)
	{
		CIvfFemWidget* femWidget = (CIvfFemWidget*) m_femWidget;
		femWidget->assignNodeBCSelected();
		femWidget->setNeedRecalc(true);
	}
};
}
void CNodeBCsDlg::cb_btnAssignBC(Fl_HoverButton* o, void* v) {
  ((CNodeBCsDlg*)(o->parent()->user_data()))->cb_btnAssignBC_i(o,v);
}

static const char *idata_tlAssignBC[] = {
"    18    18        6            1",
"` c #008100",
". c #00ff00",
"# c #810000",
"a c #ff0000",
"b c none",
"c c #ffffff",
"bbbbbbbbbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbb",
"bbbb####bbcbbcbbbb",
"bbbaaaa#bc.bc.bbbb",
"bbbaaaa#c..c..bbbb",
"bbbaaaa#b``b``bbbb",
"bbbaaaabbb`bb`bbbb",
"bbbbbcbbbbbbbbbbbb",
"bbbbc.`bbbbbbbbbbb",
"bbbc..``bbbbbbbbbb",
"bbbbbcbbbbbbbbbbbb",
"bbbbc.`bbbbbbbbbbb",
"bbbc..``bbbbbbbbbb",
"bbbbbbbbbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbb"
};
static Fl_Image *image_tlAssignBC() {
  static Fl_Image *image = new Fl_Pixmap(idata_tlAssignBC);
  return image;
}

void CNodeBCsDlg::cb_btnClearElements_i(Fl_HoverButton*, void*) {
  if (this->getCurrentBC()!=NULL)
{
	if (m_femWidget!=NULL)
	{
		CIvfFemWidget* femWidget = (CIvfFemWidget*) m_femWidget;
		femWidget->removeNodeBCsFromSelected();
	}
};
}
void CNodeBCsDlg::cb_btnClearElements(Fl_HoverButton* o, void* v) {
  ((CNodeBCsDlg*)(o->parent()->user_data()))->cb_btnClearElements_i(o,v);
}

static const char *idata_tlClearBC[] = {
"    18    18        7            1",
"` c #000081",
". c #008100",
"# c #00ff00",
"a c #810000",
"b c #ff0000",
"c c none",
"d c #ffffff",
"cccccccccccccccccc",
"cccccccccccccccccc",
"cccccccccccccccccc",
"cccccccccccccccccc",
"ccccaaaac`dcc`cccc",
"cccbbbbacd`c`#cccc",
"cccbbbbad##`##cccc",
"cccbbbbac.`c`.cccc",
"cccbbbbcc`.cc`cccc",
"cccccdcccccccccccc",
"ccc`d#.`cccccccccc",
"cccd`#`.cccccccccc",
"ccccc`cccccccccccc",
"cccc`#`ccccccccccc",
"ccc`##.`cccccccccc",
"cccccccccccccccccc",
"cccccccccccccccccc",
"cccccccccccccccccc"
};
static Fl_Image *image_tlClearBC() {
  static Fl_Image *image = new Fl_Pixmap(idata_tlClearBC);
  return image;
}

CNodeBCsDlg::CNodeBCsDlg() {
  { wndNodeBCs = new Fl_Double_Window(180, 211, "Node BCs");
    wndNodeBCs->color(FL_FOREGROUND_COLOR);
    wndNodeBCs->labelcolor(FL_BACKGROUND2_COLOR);
    wndNodeBCs->user_data((void*)(this));
    wndNodeBCs->align(Fl_Align(FL_ALIGN_CLIP|FL_ALIGN_INSIDE));
    { lbBCSet = new Fl_Browser(4, 17, 143, 190, "BCs");
      lbBCSet->type(2);
      lbBCSet->box(FL_FLAT_BOX);
      lbBCSet->color((Fl_Color)38);
      lbBCSet->labelsize(12);
      lbBCSet->labelcolor(FL_BACKGROUND2_COLOR);
      lbBCSet->textsize(12);
      lbBCSet->textcolor(FL_BACKGROUND2_COLOR);
      lbBCSet->callback((Fl_Callback*)cb_lbBCSet);
      lbBCSet->align(Fl_Align(FL_ALIGN_TOP_LEFT));
    } // Fl_Browser* lbBCSet
    { btnAddBC = new Fl_HoverButton(150, 17, 28, 28);
      btnAddBC->box(FL_THIN_UP_BOX);
      btnAddBC->down_box(FL_THIN_DOWN_BOX);
      btnAddBC->color((Fl_Color)41);
      btnAddBC->selection_color(FL_GRAY0);
      btnAddBC->image( image_tlAddBC() );
      btnAddBC->labeltype(FL_NORMAL_LABEL);
      btnAddBC->labelfont(0);
      btnAddBC->labelsize(10);
      btnAddBC->labelcolor(FL_FOREGROUND_COLOR);
      btnAddBC->callback((Fl_Callback*)cb_btnAddBC);
      btnAddBC->align(Fl_Align(FL_ALIGN_CENTER));
      btnAddBC->when(FL_WHEN_RELEASE);
    } // Fl_HoverButton* btnAddBC
    { btnRemoveBC = new Fl_HoverButton(150, 49, 28, 28);
      btnRemoveBC->box(FL_THIN_UP_BOX);
      btnRemoveBC->down_box(FL_THIN_DOWN_BOX);
      btnRemoveBC->color((Fl_Color)41);
      btnRemoveBC->selection_color(FL_BACKGROUND_COLOR);
      btnRemoveBC->image( image_tlDelBC() );
      btnRemoveBC->labeltype(FL_NORMAL_LABEL);
      btnRemoveBC->labelfont(0);
      btnRemoveBC->labelsize(10);
      btnRemoveBC->labelcolor(FL_FOREGROUND_COLOR);
      btnRemoveBC->callback((Fl_Callback*)cb_btnRemoveBC);
      btnRemoveBC->align(Fl_Align(FL_ALIGN_CENTER));
      btnRemoveBC->when(FL_WHEN_RELEASE);
    } // Fl_HoverButton* btnRemoveBC
    { btnEditBC = new Fl_HoverButton(150, 145, 28, 28);
      btnEditBC->box(FL_THIN_UP_BOX);
      btnEditBC->down_box(FL_THIN_DOWN_BOX);
      btnEditBC->color((Fl_Color)41);
      btnEditBC->selection_color(FL_BACKGROUND_COLOR);
      btnEditBC->image( image_tlInspBC() );
      btnEditBC->labeltype(FL_NORMAL_LABEL);
      btnEditBC->labelfont(0);
      btnEditBC->labelsize(10);
      btnEditBC->labelcolor(FL_FOREGROUND_COLOR);
      btnEditBC->callback((Fl_Callback*)cb_btnEditBC);
      btnEditBC->align(Fl_Align(FL_ALIGN_CENTER));
      btnEditBC->when(FL_WHEN_RELEASE);
    } // Fl_HoverButton* btnEditBC
    { btnAssignBC = new Fl_HoverButton(150, 81, 28, 28);
      btnAssignBC->box(FL_THIN_UP_BOX);
      btnAssignBC->down_box(FL_THIN_DOWN_BOX);
      btnAssignBC->color((Fl_Color)41);
      btnAssignBC->selection_color(FL_BACKGROUND_COLOR);
      btnAssignBC->image( image_tlAssignBC() );
      btnAssignBC->labeltype(FL_NORMAL_LABEL);
      btnAssignBC->labelfont(0);
      btnAssignBC->labelsize(10);
      btnAssignBC->labelcolor(FL_FOREGROUND_COLOR);
      btnAssignBC->callback((Fl_Callback*)cb_btnAssignBC);
      btnAssignBC->align(Fl_Align(FL_ALIGN_WRAP));
      btnAssignBC->when(FL_WHEN_RELEASE);
    } // Fl_HoverButton* btnAssignBC
    { btnClearElements = new Fl_HoverButton(150, 113, 28, 28);
      btnClearElements->box(FL_THIN_UP_BOX);
      btnClearElements->down_box(FL_THIN_DOWN_BOX);
      btnClearElements->color((Fl_Color)41);
      btnClearElements->selection_color(FL_BACKGROUND_COLOR);
      btnClearElements->image( image_tlClearBC() );
      btnClearElements->labeltype(FL_NORMAL_LABEL);
      btnClearElements->labelfont(0);
      btnClearElements->labelsize(10);
      btnClearElements->labelcolor(FL_FOREGROUND_COLOR);
      btnClearElements->callback((Fl_Callback*)cb_btnClearElements);
      btnClearElements->align(Fl_Align(FL_ALIGN_WRAP));
      btnClearElements->when(FL_WHEN_RELEASE);
    } // Fl_HoverButton* btnClearElements
    wndNodeBCs->set_non_modal();
    wndNodeBCs->end();
  } // Fl_Double_Window* wndNodeBCs
  m_bcSet = NULL;
  m_currentBC = NULL;
  m_femWidget = NULL;
}

void CNodeBCsDlg::show() {
  wndNodeBCs->show();
  this->setBCSet(m_bcSet);
  this->fillListBox();
}

void CNodeBCsDlg::setBCSet(CFemNodeBCSet* bcSet) {
  m_bcSet = bcSet;
  if (m_bcSet==NULL)
  {
  	lbBCSet->clear();
  	lbBCSet->deactivate();
  	btnAddBC->deactivate();
  	btnRemoveBC->deactivate();
  	btnEditBC->deactivate();
  }
  else
  {
  	lbBCSet->activate();
  	btnAddBC->activate();
  	btnRemoveBC->activate();
  	btnEditBC->activate();
  	this->fillListBox();
  	wndNodeBCs->redraw();
  }
}

CFemNodeBCSet* CNodeBCsDlg::getBCSet() {
  return m_bcSet;
}

void CNodeBCsDlg::hide() {
  wndNodeBCs->hide();
}

void CNodeBCsDlg::fillListBox() {
  if (m_bcSet!=NULL)
  {
  	std::string bcName;
  	lbBCSet->clear();
  	for (int i=0; i<m_bcSet->getSize(); i++)
  	{
  		CFemBeamNodeBC* nodeBC = 
  			(CFemBeamNodeBC*) m_bcSet->getBC(i);
  		bcName = nodeBC->getName();
  		lbBCSet->add(bcName.c_str(),nodeBC);
  	}
  	m_currentBC = NULL;
  }
}

CFemBeamNodeBC* CNodeBCsDlg::getCurrentBC() {
  return m_currentBC;
}

void CNodeBCsDlg::setFemWidget(void* femWidget) {
  m_femWidget = femWidget;
}