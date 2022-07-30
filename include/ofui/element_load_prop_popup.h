#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include <imgui.h>

#include <ofui/popup_window.h>

class FemWidget;

namespace ofui
{

class ElementLoadPropPopup : public PopupWindow
{
private:
    FemWidget* m_widget;
    std::array<char, 255> m_nameArr;
    int m_color;
    float m_force[3];

public:
    ElementLoadPropPopup(const std::string name, bool modal);
    virtual ~ElementLoadPropPopup();

    static std::shared_ptr<ElementLoadPropPopup> create(const std::string name, bool modal = true);

    void setFemWidget(FemWidget* widget);

    void update();

protected:
    virtual void doPopup();
};

typedef std::shared_ptr<ElementLoadPropPopup> ElementLoadPropPopupPtr;
}
