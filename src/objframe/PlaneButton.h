#pragma once

#include <ivf/GenericButton.h>

#include <ivf/LineSet.h>
#include <ivf/Material.h>
#include <ivf/QuadPlane.h>
#include <ivf/Texture.h>

IvfSmartPointer(PlaneButton);

class PlaneButton : public ivf::GenericButton {
private:
    ivf::QuadPlanePtr m_normalShape;
    ivf::QuadPlanePtr m_pressedShape;
    ivf::QuadPlanePtr m_disabledShape;
    ivf::CompositePtr m_checkedShape;
    ivf::QuadPlanePtr m_checkedPlane;
    ivf::LineSetPtr m_checkedFrame;
    ivf::TexturePtr m_buttonTexture;
    std::string m_hint;

    double m_width;
    double m_height;

public:
    PlaneButton(int id);
    PlaneButton(int id, const std::string &name);

    IvfClassInfo("PlaneButton", ivf::GenericButton);

    static PlaneButtonPtr create(int id)
    {
        return PlaneButtonPtr(new PlaneButton(id));
    }

    static PlaneButtonPtr create(int id, const std::string &name)
    {
        return PlaneButtonPtr(new PlaneButton(id, name));
    }

    virtual ~PlaneButton();

    void setSize(double width, double height);
    void getSize(double &width, double &height);
    void setTexture(ivf::Texture *texture);

    const std::string getHint();
    void setHint(const std::string &hintText);
};
