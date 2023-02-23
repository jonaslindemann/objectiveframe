#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ofui/ui_window.h>
namespace ofui
{

class AboutWindow : public UiWindow
{
private:
    /*
    constexpr auto OBJFRAME_VERSION_STRING = "ObjectiveFrame 2";
    constexpr auto OBJFRAME_RELEASE = "Release version - 2.0.0-b12";
    constexpr auto OBJFRAME_COPYRIGHT_STRING = "Copyright (C) 2001-2022\nDivision of Structural Mechanics, Lund university";
    constexpr auto OBJFRAME_AUTHOR1 = "Main author: Jonas Lindemann";
    constexpr auto OBJFRAME_AUTHOR2 = "Contributors: Pierre Olsson, Daniel Akesson";

    */


    std::string m_versionString;
    std::string m_release;
    std::string m_copyright;
    std::string m_author1;
    std::string m_author2;
    std::string m_extra1;

public:
    AboutWindow(const std::string name);
    virtual ~AboutWindow();

    static std::shared_ptr<AboutWindow> create(const std::string name);

    void setVersion(std::string text);
    void setRelease(std::string text);
    void setCopyright(std::string text);
    void setAuthor1(std::string text);
    void setAuthor2(std::string text);
    void setExtra1(std::string text);

protected:
    virtual void doDraw();
};

typedef std::shared_ptr<AboutWindow> AboutWindowPtr;

}
