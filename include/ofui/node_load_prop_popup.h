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

class NodeLoadPropPopup : public PopupWindow
{
private:
    FemWidget* m_widget;
    std::array<char, 255> m_nameArr;
    int m_color;
    float m_force[3];

public:
    NodeLoadPropPopup(const std::string name, bool modal);
    virtual ~NodeLoadPropPopup();

    static std::shared_ptr<NodeLoadPropPopup> create(const std::string name, bool modal = true);

    void setFemWidget(FemWidget* widget);

    void update();

protected:
    virtual void doPopup();
};

typedef std::shared_ptr<NodeLoadPropPopup> NodeLoadPropPopupPtr;

}
