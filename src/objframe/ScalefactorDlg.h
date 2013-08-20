// generated by Fast Light User Interface Designer (fluid) version 1.0100

#ifndef ScalefactorDlg_h
#define ScalefactorDlg_h
#include <FL/Fl.H>
#define MR_OK 0
#define MR_CANCEL 1
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Value_Slider.H>

class CScalefactorDlg {
    void* m_femWidget;
    double m_maxValue;
public:
    CScalefactorDlg();
    Fl_Window *wndWorkspace;
    Fl_Button *btnClose;
private:
    inline void cb_btnClose_i(Fl_Button*, void*);
    static void cb_btnClose(Fl_Button*, void*);
public:
    Fl_Group *grpWorkspace;
    Fl_Value_Slider *sldScalefactor;
private:
    inline void cb_sldScalefactor_i(Fl_Value_Slider*, void*);
    static void cb_sldScalefactor(Fl_Value_Slider*, void*);
public:
    ~CScalefactorDlg();
    void show();
    void setFemWidget(void* femWidget);
    void setScaling(double maxValue);
};
#endif
