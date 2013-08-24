// generated by Fast Light User Interface Designer (fluid) version 1.0302

#ifndef BeamPropDlg_h
#define BeamPropDlg_h
#include <FL/Fl.H>
#define MR_OK 0
#define MR_CANCEL 1
#include <IvfFemBeam.h>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Browser.H>

class CBeamPropDlg {
  CIvfFemBeam* m_beam; 
  void* m_femWidget; 
public:
  CBeamPropDlg();
  Fl_Double_Window *wndBeamProp;
  Fl_Button *btnClose;
private:
  inline void cb_btnClose_i(Fl_Button*, void*);
  static void cb_btnClose(Fl_Button*, void*);
public:
  Fl_Group *grpBeamRotation;
  Fl_Value_Slider *sldBeamRotation;
private:
  inline void cb_sldBeamRotation_i(Fl_Value_Slider*, void*);
  static void cb_sldBeamRotation(Fl_Value_Slider*, void*);
public:
  Fl_Button *btnShowMaterialDlg;
private:
  inline void cb_btnShowMaterialDlg_i(Fl_Button*, void*);
  static void cb_btnShowMaterialDlg(Fl_Button*, void*);
public:
  Fl_Box *boxDivider;
  Fl_Tabs *tabResults;
  Fl_Group *grpSectionForces;
  Fl_Browser *lbSectionForces;
  Fl_Group *grpDisplacements;
  Fl_Browser *lbDisplacements;
  void show();
  void setBeam(CIvfFemBeam* beam);
  CIvfFemBeam* getBeam();
  void hide();
  void setFemWidget(void* femWidget);
  void fillListboxes();
};
#endif
