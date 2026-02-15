#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ofui/node_load_prop_popup.h>
#include <ofui/ui_window.h>

#include <ofem/beam_node_load_set.h>

class FemViewWindow;

namespace ofui {

class NodeLoadsWindow : public UiWindow {
private:
    ofem::BeamNodeLoadSet *m_femNodeLoadSet;
    FemViewWindow *m_view;
    std::vector<bool> m_selected;
    int m_currentItemIdx;
    NodeLoadPropPopupPtr m_propPopup;

public:
    NodeLoadsWindow(const std::string name);
    virtual ~NodeLoadsWindow();

    static std::shared_ptr<NodeLoadsWindow> create(const std::string name);

    void setFemNodeLoadSet(ofem::BeamNodeLoadSet *bcSet);
    void setFemView(FemViewWindow *view);
    
protected:
    virtual void doPreDraw();
    virtual void doDraw();
    virtual void doPostDraw();
};

typedef std::shared_ptr<NodeLoadsWindow> NodeLoadsWindowPtr;

} // namespace ofui
