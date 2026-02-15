#pragma once

#include <ofui/ui_window.h>

#include <ivf/Composite.h>
#include <vfem/node.h>

class FemViewWindow;

namespace ofui {

class NodePropWindow : public UiWindow {
private:
    std::string m_name;
    vfem::Node *m_node;
    ivf::Composite *m_selectedShapes;
    FemViewWindow *m_view;
    float m_nodePos[3];
    float m_nodeDispl[3];
    float m_nodeRot[3];
    float m_nodeMove[3];
    float m_nodeReactionForces[3];
    float m_nodeReactionMoments[3];

public:
    NodePropWindow(const std::string name);
    virtual ~NodePropWindow();

    void setView(FemViewWindow *view);

    void setNode(vfem::Node *node);
    void setSelectedShapes(ivf::Composite *selected);

    static std::shared_ptr<NodePropWindow> create(const std::string name);

protected:
    virtual void doDraw();
};

typedef std::shared_ptr<NodePropWindow> NodePropWindowPtr;

} // namespace ofui
