// generated by Fast Light User Interface Designer (fluid) version 1.0400

#ifndef MainFrame_h
#define MainFrame_h
#include "FemWidget.h"
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>

class MainFrame
{
    int m_argc;
    char** m_argv;

public:
    MainFrame();
    Fl_Double_Window* wndMain;
    FemWidget* ivfWorkspace;
    void show();
    void updateToggles();
    void close();
    void setArguments(int argc, char** argv);
    ~MainFrame();
    void setProgramPath(const std::string& path);
    void show_fullscreen(int x, int y, int width, int height);
};
#endif
