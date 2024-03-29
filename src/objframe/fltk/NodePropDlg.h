// generated by Fast Light User Interface Designer (fluid) version 1.0400

#ifndef NodePropDlg_h
#define NodePropDlg_h
#include <FL/Fl.H>
#define MR_OK 0
#define MR_CANCEL 1
#include <VisFemNode.h>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Value_Input.H>

class NodePropDlg {
  VisFemNode* m_node;
  Fl_Widget* m_workspace;
public:
  NodePropDlg();
  Fl_Double_Window *wndNodeProp;
  Fl_Button *btnApply;
private:
  inline void cb_btnApply_i(Fl_Button*, void*);
  static void cb_btnApply(Fl_Button*, void*);
public:
  Fl_Group *grpWorkspace;
  Fl_Value_Input *edtXCoord;
  Fl_Value_Input *edtZCoord;
  Fl_Value_Input *edtYCoord;
  Fl_Button *btnClose;
private:
  inline void cb_btnClose_i(Fl_Button*, void*);
  static void cb_btnClose(Fl_Button*, void*);
public:
  Fl_Group *grpDisplacements;
  Fl_Value_Input *edtXDispl;
  Fl_Value_Input *edtYDispl;
  Fl_Value_Input *edtZDispl;
  void show();
  void setNode(VisFemNode* node);
  VisFemNode* getNode();
  void setWorkspace(Fl_Widget* widget);
  void hide();
};
#endif
