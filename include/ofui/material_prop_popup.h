#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include <imgui.h>

#include <ofui/popup_window.h>

class FemViewWindow;

namespace ofui {

class MaterialPropPopup : public PopupWindow {
private:
    FemViewWindow *m_view;

    std::array<char, 255> m_nameArr;
    int m_color;
    double m_E;
    double m_G;
    double m_A;
    double m_Iy;
    double m_Iz;
    double m_Kv;
    int m_section;
    int m_oldSection;

    double m_height;
    double m_width;
    double m_ufw;
    double m_lfw;
    double m_wt;
    double m_uft;
    double m_lft;
    double m_ulfw;
    double m_llfw;
    double m_outerRadius;
    double m_innerRadius;

    /*
    m_prop[0]  = height
    m_prop[1]  = width
    m_prop[2]  = UFW	(Upper Flange Width)
    m_prop[3]  = LFW	(Lower Flange Width)
    m_prop[4]  = WT		(Waist Thickness)
    m_prop[5]  = UFT	(Upper Flange Thickness)
    m_prop[6]  = LFT	(Lower Flange Thickness)
    m_prop[7]  = ULFW	(Upper Left Flange Width)
    m_prop[8]  = LLFW	(Lower Left Flange Width)
    m_prop[9]  = OuterRadius
    m_prop[10] = InnerRadius
    */

public:
    MaterialPropPopup(const std::string name, bool modal);
    virtual ~MaterialPropPopup();

    static std::shared_ptr<MaterialPropPopup> create(const std::string name, bool modal = true);

    void setFemView(FemViewWindow *view);

    void update();

    void updateMaterial();

protected:
    virtual void doPopup();
};

typedef std::shared_ptr<MaterialPropPopup> MatetrialPropPopupPtr;

} // namespace ofui
