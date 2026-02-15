#pragma once

#include <ofui/ui_window.h>

#include <vfem/beam.h>

class FemViewWindow;

namespace ofui {

class ElementPropWindow : public UiWindow {
private:
    std::string m_name;
    vfem::Beam *m_beam;
    ivf::Composite *m_selectedShapes;
    
    FemViewWindow *m_view;
    
    float m_beamRotation;
    float m_oldBeamRotation;
    int m_beamType;

public:
    ElementPropWindow(const std::string name);
    virtual ~ElementPropWindow();

    void setBeam(vfem::Beam *beam);
    void setSelectedShapes(ivf::Composite *selected);
    
    void setView(FemViewWindow *view);

    static std::shared_ptr<ElementPropWindow> create(const std::string name);

protected:
    virtual void doDraw();
};

typedef std::shared_ptr<ElementPropWindow> ElementPropWindowPtr;

} // namespace ofui
