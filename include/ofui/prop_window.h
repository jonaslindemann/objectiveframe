#pragma once

#include <ofui/ui_window.h>

#include <vfem/beam.h>

#ifdef USE_FEMVIEW
class FemViewWindow;
#else
class FemWidget;
#endif

namespace ofui {

class PropWindow : public UiWindow {
private:
    std::string m_name;
    vfem::Beam *m_beam;
    vfem::Node *m_node;
    ivf::Composite *m_selectedShapes;

#ifdef USE_FEMVIEW
    FemViewWindow *m_view;
#else
    FemWidget *m_view;
#endif
    float m_beamRotation;
    float m_oldBeamRotation;
    int m_beamType;

    float m_nodePos[3];
    float m_nodeRot[3];
    float m_nodeDispl[3];
    float m_nodeMove[3];

    float m_nodeReactionForces[3];
    float m_nodeReactionMoments[3];

public:
    PropWindow(const std::string name);
    virtual ~PropWindow();

    void setBeam(vfem::Beam *beam);
    void setSelectedShapes(ivf::Composite *selected);

    void setNode(vfem::Node *node);

#ifdef USE_FEMVIEW
    void setView(FemViewWindow *view);
#else
    void setWidget(FemWidget *widget);
#endif

    static std::shared_ptr<PropWindow> create(const std::string name);

protected:
    virtual void doDraw();
};

typedef std::shared_ptr<PropWindow> PropWindowPtr;

} // namespace ofui
