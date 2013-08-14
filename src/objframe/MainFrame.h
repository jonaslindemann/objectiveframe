// generated by Fast Light User Interface Designer (fluid) version 1.0300

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
  void cb_mnuFileNew_i(Fl_Menu_*, void*);
  static void cb_mnuFileNew(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuFileOpen;
private:
  void cb_mnuFileOpen_i(Fl_Menu_*, void*);
  static void cb_mnuFileOpen(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuFileSave;
private:
  void cb_mnuFileSave_i(Fl_Menu_*, void*);
  static void cb_mnuFileSave(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuFileSaveAs;
private:
  void cb_mnuFileSaveAs_i(Fl_Menu_*, void*);
  static void cb_mnuFileSaveAs(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuFileQuit;
private:
  void cb_mnuFileQuit_i(Fl_Menu_*, void*);
  static void cb_mnuFileQuit(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuEdit;
  static Fl_Menu_Item *mnuEditSelectAllNodes;
private:
  void cb_mnuEditSelectAllNodes_i(Fl_Menu_*, void*);
  static void cb_mnuEditSelectAllNodes(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuEditSelectAllElements;
private:
  void cb_mnuEditSelectAllElements_i(Fl_Menu_*, void*);
  static void cb_mnuEditSelectAllElements(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuMode;
  static Fl_Menu_Item *mnuModeModel;
private:
  void cb_mnuModeModel_i(Fl_Menu_*, void*);
  static void cb_mnuModeModel(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuModeGeometry;
private:
  void cb_mnuModeGeometry_i(Fl_Menu_*, void*);
  static void cb_mnuModeGeometry(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuModeResults;
private:
  void cb_mnuModeResults_i(Fl_Menu_*, void*);
  static void cb_mnuModeResults(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuModeFeedback;
private:
  void cb_mnuModeFeedback_i(Fl_Menu_*, void*);
  static void cb_mnuModeFeedback(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuTools;
  static Fl_Menu_Item *mnuToolsStructure;
private:
  void cb_mnuToolsStructure_i(Fl_Menu_*, void*);
  static void cb_mnuToolsStructure(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuCalc;
  static Fl_Menu_Item *mnuCalcExecute;
private:
  void cb_mnuCalcExecute_i(Fl_Menu_*, void*);
  static void cb_mnuCalcExecute(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuResults;
  static Fl_Menu_Item *mnuResultN;
private:
  void cb_mnuResultN_i(Fl_Menu_*, void*);
  static void cb_mnuResultN(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuResultT;
private:
  void cb_mnuResultT_i(Fl_Menu_*, void*);
  static void cb_mnuResultT(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuResultV;
private:
  void cb_mnuResultV_i(Fl_Menu_*, void*);
  static void cb_mnuResultV(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuResultM;
private:
  void cb_mnuResultM_i(Fl_Menu_*, void*);
  static void cb_mnuResultM(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuResultNavier;
private:
  void cb_mnuResultNavier_i(Fl_Menu_*, void*);
  static void cb_mnuResultNavier(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuResultNoResults;
private:
  void cb_mnuResultNoResults_i(Fl_Menu_*, void*);
  static void cb_mnuResultNoResults(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuOptions;
  static Fl_Menu_Item *mnuOptionsWorkspace;
private:
  void cb_mnuOptionsWorkspace_i(Fl_Menu_*, void*);
  static void cb_mnuOptionsWorkspace(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuOptionsScalefactor;
private:
  void cb_mnuOptionsScalefactor_i(Fl_Menu_*, void*);
  static void cb_mnuOptionsScalefactor(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuOptionsFullscreen;
private:
  void cb_mnuOptionsFullscreen_i(Fl_Menu_*, void*);
  static void cb_mnuOptionsFullscreen(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuOptionsLockScale;
private:
  void cb_mnuOptionsLockScale_i(Fl_Menu_*, void*);
  static void cb_mnuOptionsLockScale(Fl_Menu_*, void*);
public:
  static Fl_Menu_Item *mnuHelp;
  static Fl_Menu_Item *mnuHelpAbout;
private:
  void cb_mnuHelpAbout_i(Fl_Menu_*, void*);
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
