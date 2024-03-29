// generated by Fast Light User Interface Designer (fluid) version 1.0400

#ifndef MaterialsDlg_h
#define MaterialsDlg_h
#include <FL/Fl.H>
#define MR_OK 0
#define MR_CANCEL 1
#include <FemBeamMaterialSet.h>
#ifdef WIN32
#include "fl_message_win.H"
#endif
#include <FL/fl_message.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Browser.H>
#include "Fl_HoverButton.h"

class MaterialsDlg {
  CFemBeamMaterialSet* m_materials;
  void* m_femWidget;
  CFemBeamMaterial* m_currentMaterial;
public:
  MaterialsDlg();
  Fl_Double_Window *wndMaterials;
  Fl_Browser *lbMaterials;
private:
  inline void cb_lbMaterials_i(Fl_Browser*, void*);
  static void cb_lbMaterials(Fl_Browser*, void*);
public:
  Fl_HoverButton *btnAddMaterial;
private:
  inline void cb_btnAddMaterial_i(Fl_HoverButton*, void*);
  static void cb_btnAddMaterial(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnRemoveMaterial;
private:
  inline void cb_btnRemoveMaterial_i(Fl_HoverButton*, void*);
  static void cb_btnRemoveMaterial(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnEditMaterial;
private:
  inline void cb_btnEditMaterial_i(Fl_HoverButton*, void*);
  static void cb_btnEditMaterial(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnAssignMaterial;
private:
  inline void cb_btnAssignMaterial_i(Fl_HoverButton*, void*);
  static void cb_btnAssignMaterial(Fl_HoverButton*, void*);
public:
  Fl_HoverButton *btnClearMaterial;
private:
  inline void cb_btnClearMaterial_i(Fl_HoverButton*, void*);
  static void cb_btnClearMaterial(Fl_HoverButton*, void*);
public:
  void show();
  void setMaterials(CFemBeamMaterialSet* materials);
  CFemBeamMaterialSet* getMaterials();
  void hide();
private:
  void fillListBox();
public:
  CFemBeamMaterial* getCurrentMaterial();
  void setFemWidget(void* femWidget);
};
#endif
