// generated by Fast Light User Interface Designer (fluid) version 1.0300

#ifndef NodeBCsDlg_h
#define NodeBCsDlg_h
#include <FL/Fl.H>
#define MR_OK 0
#define MR_CANCEL 1
#include <FL/fl_message.H>
#include <FemNodeBCSet.h>
#include <FemBeamNodeBC.h>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Browser.H>
#include "Fl_HoverButton.h"

class CNodeBCsDlg {
    CFemNodeBCSet* m_bcSet;
    CFemBeamNodeBC* m_currentBC;
    void* m_femWidget;
public:
    CNodeBCsDlg();
    Fl_Double_Window *wndNodeBCs;
    Fl_Browser *lbBCSet;
private:
    void cb_lbBCSet_i(Fl_Browser*, void*);
    static void cb_lbBCSet(Fl_Browser*, void*);
public:
    Fl_HoverButton *btnAddBC;
private:
    void cb_btnAddBC_i(Fl_HoverButton*, void*);
    static void cb_btnAddBC(Fl_HoverButton*, void*);
public:
    Fl_HoverButton *btnRemoveBC;
private:
    void cb_btnRemoveBC_i(Fl_HoverButton*, void*);
    static void cb_btnRemoveBC(Fl_HoverButton*, void*);
public:
    Fl_HoverButton *btnEditBC;
private:
    void cb_btnEditBC_i(Fl_HoverButton*, void*);
    static void cb_btnEditBC(Fl_HoverButton*, void*);
public:
    Fl_HoverButton *btnAssignBC;
private:
    void cb_btnAssignBC_i(Fl_HoverButton*, void*);
    static void cb_btnAssignBC(Fl_HoverButton*, void*);
public:
    Fl_HoverButton *btnClearElements;
private:
    void cb_btnClearElements_i(Fl_HoverButton*, void*);
    static void cb_btnClearElements(Fl_HoverButton*, void*);
public:
    void show();
    void setBCSet(CFemNodeBCSet* bcSet);
    CFemNodeBCSet* getBCSet();
    void hide();
private:
    void fillListBox();
public:
    CFemBeamNodeBC* getCurrentBC();
    void setFemWidget(void* femWidget);
};
#endif
