// generated by Fast Light User Interface Designer (fluid) version 1.0400

#include "ElementLoadsDlg.h"
#include "FemWidget.h"
#include "ElementLoadDlg.h"

void ElementLoadsDlg::cb_lbLoadSet_i(Fl_Browser*, void*) {
  if (m_loadSet!=NULL)
{
	if (lbLoadSet->value()>0)
	{
		m_currentLoad = (BeamLoad*)
			m_loadSet->getLoad(lbLoadSet->value()-1);
	}
	else m_currentLoad = NULL;
	if (m_femWidget!=NULL)
	{
		FemWidget* femWidget = (FemWidget*) m_femWidget;
		femWidget->setCurrentBeamLoad(m_currentLoad);
	}
};
}
void ElementLoadsDlg::cb_lbLoadSet(Fl_Browser* o, void* v) {
  ((ElementLoadsDlg*)(o->parent()->user_data()))->cb_lbLoadSet_i(o,v);
}

void ElementLoadsDlg::cb_btnAddLoad_i(Fl_HoverButton*, void*) {
  if (m_loadSet!=NULL)
{
	std::string loadName;
	const char* result = fl_input("Load name");
	if (result!=NULL)
	{
		loadName = result;
		if (loadName!="")
		{
			BeamLoad* load = new BeamLoad();
			load->setName(loadName);
			load->setLocalDirection(0.0, 1.0, 0.0);
			m_loadSet->addLoad(load);
			
			if (m_femWidget!=NULL)
			{
				FemWidget* femWidget = (FemWidget*) m_femWidget;
				femWidget->addBeamLoad(load);
			}
			this->fillListBox();
		}
	}
};
}
void ElementLoadsDlg::cb_btnAddLoad(Fl_HoverButton* o, void* v) {
  ((ElementLoadsDlg*)(o->parent()->user_data()))->cb_btnAddLoad_i(o,v);
}

#include <FL/Fl_Pixmap.H>
static const char *idata_tlAddNodeLoad[] = {
"    18    18        4            1",
"` c #000081",
". c #810000",
"# c #ff0000",
"a c none",
"aaaaaaaaaaaaaaaaaa",
"aaaaa#.aaaaaa``aaa",
"aaaaa#.aaaaaa``aaa",
"aaaaa#.aaaa``````a",
"aaaaa#.aaaa``````a",
"aaaaa#.aaaaaa``aaa",
"aaaaa#.aaaaaa``aaa",
"aaaaa#.aaaaaaaaaaa",
"aaaaa#.aaaaaaaaaaa",
"aaaaa#.aaaaaaaaaaa",
"aaaaa#.aaaaaaaaaaa",
"aaaaa#.aaaaaaaaaaa",
"aaaaa#.aaaaaaaaaaa",
"aa##......aaaaaaaa",
"aaa##....aaaaaaaaa",
"aaaa##..aaaaaaaaaa",
"aaaaa#.aaaaaaaaaaa",
"aaaaaaaaaaaaaaaaaa"
};
static Fl_Image *image_tlAddNodeLoad() {
  static Fl_Image *image = new Fl_Pixmap(idata_tlAddNodeLoad);
  return image;
}

void ElementLoadsDlg::cb_btnRemoveLoad_i(Fl_HoverButton*, void*) {
  if (m_currentLoad!=NULL)
{
	if (m_femWidget!=NULL)
	{
		FemWidget* femWidget = (FemWidget*) m_femWidget;
		femWidget->deleteBeamLoad(m_currentLoad);
		this->fillListBox();
	}
};
}
void ElementLoadsDlg::cb_btnRemoveLoad(Fl_HoverButton* o, void* v) {
  ((ElementLoadsDlg*)(o->parent()->user_data()))->cb_btnRemoveLoad_i(o,v);
}

static const char *idata_tlDelNodeLoad[] = {
"    18    18        4            1",
"` c #000081",
". c #810000",
"# c #ff0000",
"a c none",
"aaaaaaaaaaaaaaaaaa",
"aaaaa#.aaaaaaaaaaa",
"aaaaa#.aaaaaaaaaaa",
"aaaaa#.aaaa``````a",
"aaaaa#.aaaa``````a",
"aaaaa#.aaaaaaaaaaa",
"aaaaa#.aaaaaaaaaaa",
"aaaaa#.aaaaaaaaaaa",
"aaaaa#.aaaaaaaaaaa",
"aaaaa#.aaaaaaaaaaa",
"aaaaa#.aaaaaaaaaaa",
"aaaaa#.aaaaaaaaaaa",
"aaaaa#.aaaaaaaaaaa",
"aa##......aaaaaaaa",
"aaa##....aaaaaaaaa",
"aaaa##..aaaaaaaaaa",
"aaaaa#.aaaaaaaaaaa",
"aaaaaaaaaaaaaaaaaa"
};
static Fl_Image *image_tlDelNodeLoad() {
  static Fl_Image *image = new Fl_Pixmap(idata_tlDelNodeLoad);
  return image;
}

void ElementLoadsDlg::cb_btnEditLoad_i(Fl_HoverButton*, void*) {
  if (this->getCurrentLoad()!=NULL)
{
	ElementLoadDlg* dlg = new ElementLoadDlg();
	dlg->setLoad(this->getCurrentLoad());
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
void ElementLoadsDlg::cb_btnEditLoad(Fl_HoverButton* o, void* v) {
  ((ElementLoadsDlg*)(o->parent()->user_data()))->cb_btnEditLoad_i(o,v);
}

static const char *idata_tlInspNodeLoad[] = {
"    18    18        6            1",
"` c #000000",
". c #000081",
"# c #810000",
"a c #ff0000",
"b c none",
"c c #ffffff",
"bbbbbbbbbbbbbbbbbb",
"bbbbba#bbbbbbbbbbb",
"bbbbba#bbbbbbbbbbb",
"bbbbba#bbbbbbbbbbb",
"bbbbba#b``````b..b",
"bbbbba#`cccccc`..b",
"bbbbba`c`cccccc..b",
"bbbbb`c`c`ccccc..b",
"bbbb`c`c`c`ccc`..b",
"bbb`c`#`c`c```b..b",
"bbbb`a#b`c`bbbbb.b",
"bbbbba#bb`bbbbbbbb",
"bbbbba#bbbbbbbbbbb",
"bbaa######bbbbbbbb",
"bbbaa####bbbbbbbbb",
"bbbbaa##bbbbbbbbbb",
"bbbbba#bbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbb"
};
static Fl_Image *image_tlInspNodeLoad() {
  static Fl_Image *image = new Fl_Pixmap(idata_tlInspNodeLoad);
  return image;
}

void ElementLoadsDlg::cb_btnAssignLoad_i(Fl_HoverButton*, void*) {
  if (this->getCurrentLoad()!=NULL)
{
	if (m_femWidget!=NULL)
	{
		FemWidget* femWidget = (FemWidget*) m_femWidget;
		femWidget->assignBeamLoadSelected();
	}
};
}
void ElementLoadsDlg::cb_btnAssignLoad(Fl_HoverButton* o, void* v) {
  ((ElementLoadsDlg*)(o->parent()->user_data()))->cb_btnAssignLoad_i(o,v);
}

static const char *idata_tlAssignNodeLoad[] = {
"    18    18        5            1",
"` c #000081",
". c #810000",
"# c #aa3000",
"a c #c20000",
"b c none",
"bbbbbbbbbbbbbbbbbb",
"bbba.bbbbbbbbbbbbb",
"bbba.bbbbbbbbbbbbb",
"bbba.bbbbbbbbbbbbb",
"bbba.bbbbbbbbbbbbb",
"bbba.bbbbbbbbbbbbb",
"bbba.bbbb`bbbb....",
"bbba.bbbb``bbaaaa.",
"bbba.b``````baaaa.",
"bbba.bbbb``bbaaaa.",
"bbba.bbbb`bbbaaaab",
"bbba.bbbbbbbbbbbbb",
"bbba.bbbbbbbbbbbbb",
"#a......bbbbbbbbbb",
"baa....bbbbbbbbbbb",
"bbaa..bbbbbbbbbbbb",
"bbba.bbbbbbbbbbbbb",
"bbbbbbbbbbbbbbbbbb"
};
static Fl_Image *image_tlAssignNodeLoad() {
  static Fl_Image *image = new Fl_Pixmap(idata_tlAssignNodeLoad);
  return image;
}

void ElementLoadsDlg::cb_btnClearElements_i(Fl_HoverButton*, void*) {
  if (this->getCurrentLoad()!=NULL)
{
	if (m_femWidget!=NULL)
	{
		FemWidget* femWidget = (FemWidget*) m_femWidget;
		femWidget->removeBeamLoadsFromSelected();
	}
};
}
void ElementLoadsDlg::cb_btnClearElements(Fl_HoverButton* o, void* v) {
  ((ElementLoadsDlg*)(o->parent()->user_data()))->cb_btnClearElements_i(o,v);
}

static const char *idata_tlClearNodes[] = {
"    18    18        4            1",
"` c #000081",
". c #810000",
"# c #ff0000",
"a c none",
"aaaaaaaa#.aaaaaaaa",
"aaaaaa``#.``aaaaaa",
"aaaaaaa````aaaaaaa",
"aaaaaaaa``aaaaaaaa",
"aaaaaaa````aaaaaaa",
"aaaaaa``#.``aaaaaa",
"aaaaaaaa#.aaaaaaaa",
"aaaaa##......aaaaa",
"aaaaaa##....aaaaaa",
"aaaaaaa##..aaaaaaa",
"aaaaaaaa#.aaaaaaaa",
"aaaaaaaaaaaaaaaaaa",
"aaaaaaa....aaaaaaa",
"aaaaaa####.aaaaaaa",
"aaaaaa####.aaaaaaa",
"aaaaaa####.aaaaaaa",
"aaaaaa####aaaaaaaa",
"aaaaaaaaaaaaaaaaaa"
};
static Fl_Image *image_tlClearNodes() {
  static Fl_Image *image = new Fl_Pixmap(idata_tlClearNodes);
  return image;
}

ElementLoadsDlg::ElementLoadsDlg() {
  { wndElementLoads = new Fl_Double_Window(181, 211, "Element loads");
    wndElementLoads->user_data((void*)(this));
    { lbLoadSet = new Fl_Browser(4, 17, 144, 189, "Loads");
      lbLoadSet->type(2);
      lbLoadSet->box(FL_DOWN_BOX);
      lbLoadSet->labelsize(12);
      lbLoadSet->textsize(12);
      lbLoadSet->callback((Fl_Callback*)cb_lbLoadSet);
      lbLoadSet->align(Fl_Align(FL_ALIGN_TOP_LEFT));
    } // Fl_Browser* lbLoadSet
    { btnAddLoad = new Fl_HoverButton(150, 17, 28, 28);
      btnAddLoad->box(FL_THIN_UP_BOX);
      btnAddLoad->down_box(FL_THIN_DOWN_BOX);
      btnAddLoad->color(FL_BACKGROUND_COLOR);
      btnAddLoad->selection_color(FL_BACKGROUND_COLOR);
      btnAddLoad->image( image_tlAddNodeLoad() );
      btnAddLoad->labeltype(FL_NORMAL_LABEL);
      btnAddLoad->labelfont(0);
      btnAddLoad->labelsize(10);
      btnAddLoad->labelcolor(FL_FOREGROUND_COLOR);
      btnAddLoad->callback((Fl_Callback*)cb_btnAddLoad);
      btnAddLoad->align(Fl_Align(FL_ALIGN_CENTER));
      btnAddLoad->when(FL_WHEN_RELEASE);
    } // Fl_HoverButton* btnAddLoad
    { btnRemoveLoad = new Fl_HoverButton(150, 48, 28, 28);
      btnRemoveLoad->box(FL_THIN_UP_BOX);
      btnRemoveLoad->down_box(FL_THIN_DOWN_BOX);
      btnRemoveLoad->color(FL_BACKGROUND_COLOR);
      btnRemoveLoad->selection_color(FL_BACKGROUND_COLOR);
      btnRemoveLoad->image( image_tlDelNodeLoad() );
      btnRemoveLoad->labeltype(FL_NORMAL_LABEL);
      btnRemoveLoad->labelfont(0);
      btnRemoveLoad->labelsize(10);
      btnRemoveLoad->labelcolor(FL_FOREGROUND_COLOR);
      btnRemoveLoad->callback((Fl_Callback*)cb_btnRemoveLoad);
      btnRemoveLoad->align(Fl_Align(FL_ALIGN_CENTER));
      btnRemoveLoad->when(FL_WHEN_RELEASE);
    } // Fl_HoverButton* btnRemoveLoad
    { btnEditLoad = new Fl_HoverButton(150, 145, 28, 28);
      btnEditLoad->box(FL_THIN_UP_BOX);
      btnEditLoad->down_box(FL_THIN_DOWN_BOX);
      btnEditLoad->color(FL_BACKGROUND_COLOR);
      btnEditLoad->selection_color(FL_BACKGROUND_COLOR);
      btnEditLoad->image( image_tlInspNodeLoad() );
      btnEditLoad->labeltype(FL_NORMAL_LABEL);
      btnEditLoad->labelfont(0);
      btnEditLoad->labelsize(10);
      btnEditLoad->labelcolor(FL_FOREGROUND_COLOR);
      btnEditLoad->callback((Fl_Callback*)cb_btnEditLoad);
      btnEditLoad->align(Fl_Align(FL_ALIGN_CENTER));
      btnEditLoad->when(FL_WHEN_RELEASE);
    } // Fl_HoverButton* btnEditLoad
    { btnAssignLoad = new Fl_HoverButton(150, 81, 28, 28);
      btnAssignLoad->box(FL_THIN_UP_BOX);
      btnAssignLoad->down_box(FL_THIN_DOWN_BOX);
      btnAssignLoad->color(FL_BACKGROUND_COLOR);
      btnAssignLoad->selection_color(FL_BACKGROUND_COLOR);
      btnAssignLoad->image( image_tlAssignNodeLoad() );
      btnAssignLoad->labeltype(FL_NORMAL_LABEL);
      btnAssignLoad->labelfont(0);
      btnAssignLoad->labelsize(10);
      btnAssignLoad->labelcolor(FL_FOREGROUND_COLOR);
      btnAssignLoad->callback((Fl_Callback*)cb_btnAssignLoad);
      btnAssignLoad->align(Fl_Align(FL_ALIGN_WRAP));
      btnAssignLoad->when(FL_WHEN_RELEASE);
    } // Fl_HoverButton* btnAssignLoad
    { btnClearElements = new Fl_HoverButton(150, 113, 28, 28);
      btnClearElements->box(FL_THIN_UP_BOX);
      btnClearElements->down_box(FL_THIN_DOWN_BOX);
      btnClearElements->color(FL_BACKGROUND_COLOR);
      btnClearElements->selection_color(FL_BACKGROUND_COLOR);
      btnClearElements->image( image_tlClearNodes() );
      btnClearElements->labeltype(FL_NORMAL_LABEL);
      btnClearElements->labelfont(0);
      btnClearElements->labelsize(10);
      btnClearElements->labelcolor(FL_FOREGROUND_COLOR);
      btnClearElements->callback((Fl_Callback*)cb_btnClearElements);
      btnClearElements->align(Fl_Align(FL_ALIGN_WRAP));
      btnClearElements->when(FL_WHEN_RELEASE);
    } // Fl_HoverButton* btnClearElements
    wndElementLoads->set_non_modal();
    wndElementLoads->end();
  } // Fl_Double_Window* wndElementLoads
  m_loadSet = NULL;
  m_currentLoad = NULL;
  m_femWidget = NULL;
}

void ElementLoadsDlg::show() {
  wndElementLoads->show();
  this->setLoadSet(m_loadSet);
  this->fillListBox();
}

void ElementLoadsDlg::setLoadSet(ElementLoadSet* loadSet) {
  m_loadSet = loadSet;
  if (m_loadSet==NULL)
  {
  	lbLoadSet->clear();
  	lbLoadSet->deactivate();
  	btnAddLoad->deactivate();
  	btnRemoveLoad->deactivate();
  	btnEditLoad->deactivate();
  }
  else
  {
  	lbLoadSet->activate();
  	btnAddLoad->activate();
  	btnRemoveLoad->activate();
  	btnEditLoad->activate();
  	this->fillListBox();
  	wndElementLoads->redraw();
  }
}

ElementLoadSet* ElementLoadsDlg::getLoadSet() {
  return m_loadSet;
}

void ElementLoadsDlg::hide() {
  wndElementLoads->hide();
}

void ElementLoadsDlg::fillListBox() {
  if (m_loadSet!=NULL)
  {
  	std::string loadName;
  	lbLoadSet->clear();
  	for (int i=0; i<m_loadSet->getSize(); i++)
  	{
  		BeamLoad* beamLoad = 
  			(BeamLoad*) m_loadSet->getLoad(i);
  		loadName = beamLoad->getName();
  		lbLoadSet->add(loadName.c_str(), beamLoad);
  	}
  	m_currentLoad = NULL;
  }
}

BeamLoad* ElementLoadsDlg::getCurrentLoad() {
  return m_currentLoad;
}

void ElementLoadsDlg::setFemWidget(void* femWidget) {
  m_femWidget = femWidget;
}
