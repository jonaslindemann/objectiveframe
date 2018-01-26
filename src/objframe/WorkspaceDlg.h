// generated by Fast Light User Interface Designer (fluid) version 1.0400

#ifndef WorkspaceDlg_h
#define WorkspaceDlg_h
#include <FL/Fl.H>
#define MR_OK 0
#define MR_CANCEL 1
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Value_Slider.H>

class CWorkspaceDlg {
  int m_modalResult; 
  double m_workspaceSize; 
public:
  CWorkspaceDlg();
  Fl_Double_Window *wndWorkspace;
  Fl_Button *btnOk;
private:
  inline void cb_btnOk_i(Fl_Button*, void*);
  static void cb_btnOk(Fl_Button*, void*);
public:
  Fl_Button *btnCancel;
private:
  inline void cb_btnCancel_i(Fl_Button*, void*);
  static void cb_btnCancel(Fl_Button*, void*);
public:
  Fl_Group *grpWorkspace;
  Fl_Value_Slider *sldWorkspaceSize;
  Fl_Value_Slider *sldNodeSize;
  Fl_Value_Slider *sldLineRadius;
  Fl_Value_Slider *sldLoadSize;
  ~CWorkspaceDlg();
  void show();
  int getModalResult();
  void setWorkspaceSize(double size);
  double getWorkspaceSize();
  void setNodeSize(double size);
  void setLineRadius(double radius);
  void setLoadSize(double size);
  double getNodeSize();
  double getLineRadius();
  double getLoadSize();
};
#endif