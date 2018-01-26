// generated by Fast Light User Interface Designer (fluid) version 1.0400

#ifndef MainFrame_h
#define MainFrame_h
#include <FL/Fl.H>
#include "WorkspaceDlg.h"
#include "SplashFrame.h"
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include "IvfFemWidget.h"

class CMainFrame {
public:
  CNodePropDlg* m_dlgNodeProp; 
private:
  CBeamPropDlg* m_dlgBeamProp; 
  CMaterialsDlg* m_dlgMaterials; 
  int m_argc; 
  char** m_argv; 
  CSplashFrame* m_splash; 
public:
  CMainFrame();
  Fl_Double_Window *wndMain;
  Fl_Menu_Bar *mnuMain;
  static Fl_Menu_Item menu_mnuMain[];
  static Fl_Menu_Item *mnuFile;
  static Fl_Menu_Item *mnuFileNew;
private:
  inline void cb_mnuFileNew_i(Fl_Menu_*, void*);
  static void cb_mnuFileNew(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuFileOpen;
private:
  inline void cb_mnuFileOpen_i(Fl_Menu_*, void*);
  static void cb_mnuFileOpen(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuFileSave;
private:
  inline void cb_mnuFileSave_i(Fl_Menu_*, void*);
  static void cb_mnuFileSave(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuFileSaveAs;
private:
  inline void cb_mnuFileSaveAs_i(Fl_Menu_*, void*);
  static void cb_mnuFileSaveAs(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuFileQuit;
private:
  inline void cb_mnuFileQuit_i(Fl_Menu_*, void*);
  static void cb_mnuFileQuit(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuEdit;
  static Fl_Menu_Item *mnuEditSelectAllNodes;
private:
  inline void cb_mnuEditSelectAllNodes_i(Fl_Menu_*, void*);
  static void cb_mnuEditSelectAllNodes(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuEditSelectAllElements;
private:
  inline void cb_mnuEditSelectAllElements_i(Fl_Menu_*, void*);
  static void cb_mnuEditSelectAllElements(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuEditFixSelected;
private:
  inline void cb_mnuEditFixSelected_i(Fl_Menu_*, void*);
  static void cb_mnuEditFixSelected(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuEditFixPosSelected;
private:
  inline void cb_mnuEditFixPosSelected_i(Fl_Menu_*, void*);
  static void cb_mnuEditFixPosSelected(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuEditFixGround;
private:
  inline void cb_mnuEditFixGround_i(Fl_Menu_*, void*);
  static void cb_mnuEditFixGround(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuEditPosGround;
private:
  inline void cb_mnuEditPosGround_i(Fl_Menu_*, void*);
  static void cb_mnuEditPosGround(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuMode;
  static Fl_Menu_Item *mnuModeModel;
private:
  inline void cb_mnuModeModel_i(Fl_Menu_*, void*);
  static void cb_mnuModeModel(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuModeGeometry;
private:
  inline void cb_mnuModeGeometry_i(Fl_Menu_*, void*);
  static void cb_mnuModeGeometry(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuModeResults;
private:
  inline void cb_mnuModeResults_i(Fl_Menu_*, void*);
  static void cb_mnuModeResults(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuModeFeedback;
private:
  inline void cb_mnuModeFeedback_i(Fl_Menu_*, void*);
  static void cb_mnuModeFeedback(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuTools;
  static Fl_Menu_Item *mnuToolsStructure;
private:
  inline void cb_mnuToolsStructure_i(Fl_Menu_*, void*);
  static void cb_mnuToolsStructure(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuCalc;
  static Fl_Menu_Item *mnuCalcExecute;
private:
  inline void cb_mnuCalcExecute_i(Fl_Menu_*, void*);
  static void cb_mnuCalcExecute(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuResults;
  static Fl_Menu_Item *mnuResultN;
private:
  inline void cb_mnuResultN_i(Fl_Menu_*, void*);
  static void cb_mnuResultN(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuResultT;
private:
  inline void cb_mnuResultT_i(Fl_Menu_*, void*);
  static void cb_mnuResultT(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuResultV;
private:
  inline void cb_mnuResultV_i(Fl_Menu_*, void*);
  static void cb_mnuResultV(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuResultM;
private:
  inline void cb_mnuResultM_i(Fl_Menu_*, void*);
  static void cb_mnuResultM(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuResultNavier;
private:
  inline void cb_mnuResultNavier_i(Fl_Menu_*, void*);
  static void cb_mnuResultNavier(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuResultNoResults;
private:
  inline void cb_mnuResultNoResults_i(Fl_Menu_*, void*);
  static void cb_mnuResultNoResults(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuOptions;
  static Fl_Menu_Item *mnuOptionsWorkspace;
private:
  inline void cb_mnuOptionsWorkspace_i(Fl_Menu_*, void*);
  static void cb_mnuOptionsWorkspace(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuOptionsScalefactor;
private:
  inline void cb_mnuOptionsScalefactor_i(Fl_Menu_*, void*);
  static void cb_mnuOptionsScalefactor(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuOptionsFullscreen;
private:
  inline void cb_mnuOptionsFullscreen_i(Fl_Menu_*, void*);
  static void cb_mnuOptionsFullscreen(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuOptionsLockScale;
private:
  inline void cb_mnuOptionsLockScale_i(Fl_Menu_*, void*);
  static void cb_mnuOptionsLockScale(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuHelp;
  static Fl_Menu_Item *mnuHelpAbout;
private:
  inline void cb_mnuHelpAbout_i(Fl_Menu_*, void*);
  static void cb_mnuHelpAbout(Fl_Menu_*, void*);
public:
  CIvfFemWidget *ivfWorkspace;
  void show();
  void updateToggles();
  void close();
  void setArguments(int argc, char** argv);
  ~CMainFrame();
  void setProgramPath(const std::string& path);
  void show_fullscreen(int x, int y, int width, int height);
};
#endif
