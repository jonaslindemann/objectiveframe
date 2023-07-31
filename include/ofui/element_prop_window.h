#pragma once

#include <ofui/ui_window.h>

#include <vfem/beam.h>

#ifdef USE_FEMVIEW
class FemViewWindow;
#else
class FemWidget;
#endif

namespace ofui {

class ElementPropWindow : public UiWindow {
private:
    std::string m_name;
    vfem::Beam *m_beam;
    ivf::Composite *m_selectedShapes;
#ifdef USE_FEMVIEW
    FemViewWindow *m_view;
#else
    FemWidget *m_view;
#endif
    float m_beamRotation;
    float m_oldBeamRotation;
    int m_beamType;

public:
    ElementPropWindow(const std::string name);
    virtual ~ElementPropWindow();

    void setBeam(vfem::Beam *beam);
    void setSelectedShapes(ivf::Composite *selected);
#ifdef USE_FEMVIEW
    void setView(FemViewWindow *view);
#else
    void setWidget(FemWidget *widget);
#endif

    static std::shared_ptr<ElementPropWindow> create(const std::string name);

protected:
    virtual void doDraw();
};

typedef std::shared_ptr<ElementPropWindow> ElementPropWindowPtr;

} // namespace ofui
