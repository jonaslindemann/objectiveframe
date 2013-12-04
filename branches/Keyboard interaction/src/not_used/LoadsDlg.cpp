// generated by Fast Light User Interface Designer (fluid) version 1.0100

#include "LoadsDlg.h"
#include "IvfFemWidget.h"

inline void CMaterialsDlg::cb_btnClose_i(Fl_Button*, void*) {
  wndMaterials->hide();
}
void CMaterialsDlg::cb_btnClose(Fl_Button* o, void* v) {
  ((CMaterialsDlg*)(o->parent()->user_data()))->cb_btnClose_i(o,v);
}

inline void CMaterialsDlg::cb_lbMaterials_i(Fl_Browser*, void*) {
  if (m_materials!=NULL)
{
	if (lbMaterials->value()>0)
	{
		m_currentMaterial = (CFemBeamMaterial*)
			m_materials->getMaterial(lbMaterials->value()-1);
	}
	else m_currentMaterial = NULL;
	if (m_workspace!=NULL)
	{
		CIvfFemWidget* femWidget = (CIvfFemWidget*) m_workspace;
		femWidget->setCurrentMaterial(m_currentMaterial);
	}
};
}
void CMaterialsDlg::cb_lbMaterials(Fl_Browser* o, void* v) {
  ((CMaterialsDlg*)(o->parent()->user_data()))->cb_lbMaterials_i(o,v);
}

inline void CMaterialsDlg::cb_btnAddMaterial_i(Fl_Button*, void*) {
  if (m_materials!=NULL)
{
	char materialName[255];
	strcpy(materialName, fl_input("Material name"));
	if (materialName!=NULL)
	{
		CFemBeamMaterial* material = new CFemBeamMaterial();
		material->setName(materialName);
		m_materials->addMaterial(material);
		this->fillListBox();
	}
};
}
void CMaterialsDlg::cb_btnAddMaterial(Fl_Button* o, void* v) {
  ((CMaterialsDlg*)(o->parent()->user_data()))->cb_btnAddMaterial_i(o,v);
}

#include <FL/Fl_Pixmap.H>
static const char *idata_tlAddMaterial[] = {
"    18    18       16            1",
"` c #000000",
". c #800000",
"# c #008000",
"a c None",
"b c #000080",
"c c #800080",
"d c #008080",
"e c #c0c0c0",
"f c #808080",
"g c #ff0000",
"h c #00ff00",
"i c #ffff00",
"j c #0000ff",
"k c #ff00ff",
"l c #00ffff",
"m c #ffffff",
"aaaaaaaaaaaaaaaaaa",
"aaaaaaaaaaaaabbaaa",
"aaaaaaaaaaaaabbaaa",
"aaaaaaaaaaabbbbbba",
"aaaammmmmmmbbbbbba",
"aaammmmmmmmmfbbaaa",
"aammmmmmmmmffbbaaa",
"aameeeeeeeefffaaaa",
"aameeeeeeeefffaaaa",
"aameeeeeeeefffaaaa",
"aameeeeeeeefffaaaa",
"aameeeeeeeefffaaaa",
"aameeeeeeeefffaaaa",
"aameeeeeeeeffaaaaa",
"aameeeeeeeffaaaaaa",
"aafffffffffaaaaaaa",
"aaaaaaaaaaaaaaaaaa",
"aaaaaaaaaaaaaaaaaa"
};
static Fl_Pixmap image_tlAddMaterial(idata_tlAddMaterial);

inline void CMaterialsDlg::cb_btnRemoveMaterial_i(Fl_Button*, void*) {
  if (m_currentMaterial!=NULL)
{
	if (m_materials->removeMaterial(m_currentMaterial))
		this->fillListBox();
	else
		fl_message("Material is used and can not be removed. Remove material references from elements.");
};
}
void CMaterialsDlg::cb_btnRemoveMaterial(Fl_Button* o, void* v) {
  ((CMaterialsDlg*)(o->parent()->user_data()))->cb_btnRemoveMaterial_i(o,v);
}

static const char *idata_tlDeleteMaterial[] = {
"    18    18       16            1",
"` c #000000",
". c #800000",
"# c #008000",
"a c None",
"b c #000080",
"c c #800080",
"d c #008080",
"e c #c0c0c0",
"f c #808080",
"g c #ff0000",
"h c #00ff00",
"i c #ffff00",
"j c #0000ff",
"k c #ff00ff",
"l c #00ffff",
"m c #ffffff",
"aaaaaaaaaaaaaaaaaa",
"aaaaaaaaaaaaaaaaaa",
"aaaaaaaaaaaaaaaaaa",
"aaaaaaaaaaabbbbbba",
"aaaammmmmmmbbbbbba",
"aaammmmmmmmmffaaaa",
"aammmmmmmmmfffaaaa",
"aameeeeeeeefffaaaa",
"aameeeeeeeefffaaaa",
"aameeeeeeeefffaaaa",
"aameeeeeeeefffaaaa",
"aameeeeeeeefffaaaa",
"aameeeeeeeefffaaaa",
"aameeeeeeeeffaaaaa",
"aameeeeeeeffaaaaaa",
"aafffffffffaaaaaaa",
"aaaaaaaaaaaaaaaaaa",
"aaaaaaaaaaaaaaaaaa"
};
static Fl_Pixmap image_tlDeleteMaterial(idata_tlDeleteMaterial);

inline void CMaterialsDlg::cb_btnEditMaterial_i(Fl_Button*, void*) {
  if (this->getCurrentMaterial()!=NULL)
{
	CMaterialDlg* dlg = new CMaterialDlg();
	dlg->setMaterial(this->getCurrentMaterial());
	dlg->setWorkspace(m_workspace);
	dlg->show();
	if (dlg->getModalResult()==MR_OK)
	{
		//ivfWorkspace->setWorkspace(dlg->getWorkspaceSize());
	}
	delete dlg;
};
}
void CMaterialsDlg::cb_btnEditMaterial(Fl_Button* o, void* v) {
  ((CMaterialsDlg*)(o->parent()->user_data()))->cb_btnEditMaterial_i(o,v);
}

CMaterialsDlg::CMaterialsDlg() {
  Fl_Window* w;
  { Fl_Window* o = wndMaterials = new Fl_Window(184, 211, "Materials");
    w = o;
    o->user_data((void*)(this));
    { Fl_Button* o = btnClose = new Fl_Button(64, 183, 61, 25, "Close");
      o->box(FL_PLASTIC_UP_BOX);
      o->down_box(FL_PLASTIC_DOWN_BOX);
      o->callback((Fl_Callback*)cb_btnClose);
    }
    { Fl_Browser* o = lbMaterials = new Fl_Browser(4, 20, 144, 156, "Materials");
      o->type(2);
      o->box(FL_PLASTIC_DOWN_BOX);
      o->callback((Fl_Callback*)cb_lbMaterials);
      o->align(FL_ALIGN_TOP_LEFT);
    }
    { Fl_Button* o = btnAddMaterial = new Fl_Button(152, 20, 28, 28);
      o->box(FL_THIN_UP_BOX);
      o->down_box(FL_THIN_DOWN_BOX);
      o->image(image_tlAddMaterial);
      o->callback((Fl_Callback*)cb_btnAddMaterial);
    }
    { Fl_Button* o = btnRemoveMaterial = new Fl_Button(152, 52, 28, 28);
      o->box(FL_THIN_UP_BOX);
      o->down_box(FL_THIN_DOWN_BOX);
      o->image(image_tlDeleteMaterial);
      o->callback((Fl_Callback*)cb_btnRemoveMaterial);
    }
    { Fl_Button* o = btnEditMaterial = new Fl_Button(152, 84, 28, 28);
      o->box(FL_THIN_UP_BOX);
      o->down_box(FL_THIN_DOWN_BOX);
      o->image(image_tlDeleteMaterial);
      o->callback((Fl_Callback*)cb_btnEditMaterial);
    }
    o->set_non_modal();
    o->end();
  }
  m_materials = NULL;
m_currentMaterial = NULL;
}

void CMaterialsDlg::show() {
  wndMaterials->show();
this->setMaterials(m_materials);
this->fillListBox();
}

void CMaterialsDlg::setMaterials(CFemBeamMaterialSet* materials) {
  m_materials = materials;
if (m_materials==NULL)
{
	lbMaterials->clear();
	lbMaterials->deactivate();
	btnAddMaterial->deactivate();
	btnRemoveMaterial->deactivate();
	btnEditMaterial->deactivate();
}
else
{
	lbMaterials->activate();
	btnAddMaterial->activate();
	btnRemoveMaterial->activate();
	btnEditMaterial->activate();
	this->fillListBox();
	wndMaterials->redraw();
}
}

CFemBeamMaterialSet* CMaterialsDlg::getMaterials() {
  return m_materials;
}

void CMaterialsDlg::setWorkspace(Fl_Widget* widget) {
  m_workspace = widget;
}

void CMaterialsDlg::hide() {
  wndMaterials->hide();
}

void CMaterialsDlg::fillListBox() {
  if (m_materials!=NULL)
{
	char materialName[255];
	lbMaterials->clear();
	for (int i=0; i<m_materials->getSize(); i++)
	{
		CFemBeamMaterial* material = 
			(CFemBeamMaterial*)m_materials->getMaterial(i);
		material->getName(materialName);
		lbMaterials->add(materialName, material);
	}
}
}

CFemBeamMaterial* CMaterialsDlg::getCurrentMaterial() {
  return m_currentMaterial;
}
