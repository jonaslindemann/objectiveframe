#ifndef _CIvfPlaneButton_h_
#define _CIvfPlaneButton_h_

#include <ivf/IvfGenericButton.h>

#include <ivf/IvfQuadPlane.h>
#include <ivf/IvfMaterial.h>
#include <ivf/IvfTexture.h>
#include <ivf/IvfLineSet.h>

class CIvfPlaneButton : public CIvfGenericButton {
private:
    CIvfQuadPlanePtr m_normalShape;
    CIvfQuadPlanePtr m_pressedShape;
    CIvfQuadPlanePtr m_disabledShape;
    CIvfCompositePtr m_checkedShape;
    CIvfQuadPlanePtr m_checkedPlane;
    CIvfLineSetPtr m_checkedFrame;
    CIvfTexturePtr m_buttonTexture;
    std::string m_hint;
public:
    const std::string getHint();
    void setHint(const std::string& hintText);
    CIvfPlaneButton(int id);
    CIvfPlaneButton(int id, const std::string& name);
    virtual ~CIvfPlaneButton();

    void setSize(double width, double height);
    void setTexture(CIvfTexture* texture);
};

#endif
