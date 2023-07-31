#pragma once

#include <ofui/ui_window.h>

#include <ivf/Composite.h>
#include <vfem/node.h>

#ifdef USE_FEMVIEW
class FemViewWindow;
#else
class FemWidget;
#endif

namespace ofui {

class NodePropWindow : public UiWindow {
private:
    std::string m_name;
    vfem::Node *m_node;
    ivf::Composite *m_selectedShapes;
#ifdef USE_FEMVIEW
    FemViewWindow *m_view;
#else
    FemWidget *m_view;
#endif
    float m_nodePos[3];
    float m_nodeDispl[3];
    float m_nodeMove[3];

public:
    NodePropWindow(const std::string name);
    virtual ~NodePropWindow();

#ifdef USE_FEMVIEW
    void setView(FemViewWindow *view);
#else
    void setWidget(FemWidget *widget);
#endif

    void setNode(vfem::Node *node);
    void setSelectedShapes(ivf::Composite *selected);

    static std::shared_ptr<NodePropWindow> create(const std::string name);

protected:
    virtual void doDraw();
};

typedef std::shared_ptr<NodePropWindow> NodePropWindowPtr;

} // namespace ofui
