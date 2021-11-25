#ifndef _CIvfPlaneButton_h_
#define _CIvfPlaneButton_h_

#include <ivf/GenericButton.h>

#include <ivf/QuadPlane.h>
#include <ivf/Material.h>
#include <ivf/Texture.h>
#include <ivf/LineSet.h>

class CIvfPlaneButton : public ivf::GenericButton {
private:
    ivf::QuadPlanePtr m_normalShape;
    ivf::QuadPlanePtr m_pressedShape;
    ivf::QuadPlanePtr m_disabledShape;
    ivf::CompositePtr m_checkedShape;
    ivf::QuadPlanePtr m_checkedPlane;
    ivf::LineSetPtr m_checkedFrame;
    ivf::TexturePtr m_buttonTexture;
    std::string m_hint;
public:
    CIvfPlaneButton(int id);
    CIvfPlaneButton(int id, const std::string& name);

    virtual ~CIvfPlaneButton();


    void setSize(double width, double height);
    void setTexture(ivf::Texture* texture);

    const std::string getHint();
    void setHint(const std::string& hintText);
};

#endif
