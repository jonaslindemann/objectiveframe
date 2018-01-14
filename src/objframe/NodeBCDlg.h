// generated by Fast Light User Interface Designer (fluid) version 1.0400

#ifndef NodeBCDlg_h
#define NodeBCDlg_h
#include <FL/Fl.H>
#define MR_OK 0
#define MR_CANCEL 1
#include <FemBeamNodeBC.h>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Light_Button.H>
#include <FL/Fl_Value_Input.H>

class CNodeBCDlg {
  CFemBeamNodeBC* m_bc; 
  Fl_Widget* m_workspace; 
  int m_modalResult; 
  Fl_Color m_colorIndex; 
public:
  CNodeBCDlg();
  Fl_Double_Window *wndNodeBC;
  Fl_Button *btnApply;
private:
  inline void cb_btnApply_i(Fl_Button*, void*);
  static void cb_btnApply(Fl_Button*, void*);
public:
  Fl_Button *btnClose;
private:
  inline void cb_btnClose_i(Fl_Button*, void*);
  static void cb_btnClose(Fl_Button*, void*);
public:
  Fl_Tabs *tbElementBC;
  Fl_Group *grpGeneral;
  Fl_Input *edtName;
  Fl_Box *boxColor;
  Fl_Button *btnSetColor;
private:
  inline void cb_btnSetColor_i(Fl_Button*, void*);
  static void cb_btnSetColor(Fl_Button*, void*);
public:
  Fl_Group *grpData;
  Fl_Light_Button *btnXdispl;
private:
  inline void cb_btnXdispl_i(Fl_Light_Button*, void*);
  static void cb_btnXdispl(Fl_Light_Button*, void*);
public:
  Fl_Light_Button *btnYdispl;
private:
  inline void cb_btnYdispl_i(Fl_Light_Button*, void*);
  static void cb_btnYdispl(Fl_Light_Button*, void*);
public:
  Fl_Light_Button *btnZdispl;
private:
  inline void cb_btnZdispl_i(Fl_Light_Button*, void*);
  static void cb_btnZdispl(Fl_Light_Button*, void*);
public:
  Fl_Light_Button *btnXrot;
private:
  inline void cb_btnXrot_i(Fl_Light_Button*, void*);
  static void cb_btnXrot(Fl_Light_Button*, void*);
public:
  Fl_Light_Button *btnYrot;
private:
  inline void cb_btnYrot_i(Fl_Light_Button*, void*);
  static void cb_btnYrot(Fl_Light_Button*, void*);
public:
  Fl_Light_Button *btnZrot;
private:
  inline void cb_btnZrot_i(Fl_Light_Button*, void*);
  static void cb_btnZrot(Fl_Light_Button*, void*);
public:
  Fl_Value_Input *edtZdispl;
  Fl_Value_Input *edtYdispl;
  Fl_Value_Input *edtXdispl;
  Fl_Value_Input *edtXrot;
  Fl_Value_Input *edtYrot;
  Fl_Value_Input *edtZrot;
  void show();
  void setBC(CFemBeamNodeBC* bc);
  CFemBeamNodeBC* getBC();
  void setWorkspace(Fl_Widget* widget);
  void hide();
  int getModalResult();
};
#endif
