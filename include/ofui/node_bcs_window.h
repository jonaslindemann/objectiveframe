#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ofui/bc_prop_popup.h>
#include <ofui/ui_window.h>

#include <ofem/beam_node_bc_set.h>

#ifdef USE_FEMVIEW
class FemViewWindow;
#else
class FemWidget;
#endif

namespace ofui {

class NodeBCsWindow : public UiWindow {
private:
    ofem::BeamNodeBCSet *m_femNodeBCSet;
#ifdef USE_FEMVIEW
    FemViewWindow *m_view;
#else
    FemWidget *m_view;
#endif
    std::vector<bool> m_selected;
    int m_currentItemIdx;
    BCPropPopupPtr m_propPopup;

public:
    NodeBCsWindow(const std::string name);
    virtual ~NodeBCsWindow();

    static std::shared_ptr<NodeBCsWindow> create(const std::string name);

    void setFemNodeBCSet(ofem::BeamNodeBCSet *bcSet);
#ifdef USE_FEMVIEW
    void setFemView(FemViewWindow *view);
#else
    void setFemWidget(FemWidget *widget);
#endif

protected:
    virtual void doPreDraw();
    virtual void doDraw();
    virtual void doPostDraw();
};

typedef std::shared_ptr<NodeBCsWindow> NodeBCsWindowPtr;

} // namespace ofui
