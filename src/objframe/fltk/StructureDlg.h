// generated by Fast Light User Interface Designer (fluid) version 1.0400

#ifndef StructureDlg_h
#define StructureDlg_h
#include <FL/Fl.H>
#define MR_OK 0
#define MR_CANCEL 1
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_Value_Input.H>

class StructureDlg {
  int m_modalResult;
  int m_size[3];
  double m_spacing[3];
public:
  StructureDlg();
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
  Fl_Value_Slider *sldRows;
  Fl_Value_Slider *sldColumns;
  Fl_Value_Slider *sldStacks;
  Fl_Value_Input *edtZspacing;
  Fl_Value_Input *edtXspacing;
  Fl_Value_Input *edtYspacing;
  void show();
  int getModalResult();
  ~StructureDlg();
  void getSize(int &rows, int &cols, int &stacks);
  void getSpacing(double &xSpacing, double &ySpacing, double &zSpacing);
};
#endif
