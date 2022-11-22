#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ofui/node_load_prop_popup.h>
#include <ofui/ui_window.h>

#include <ofem/beam_node_load_set.h>

#ifdef USE_FEMVIEW
class FemViewWindow;
#else
class FemWidget;
#endif

namespace ofui
{

class NodeLoadsWindow : public UiWindow
{
private:
    ofem::BeamNodeLoadSet* m_femNodeLoadSet;
#ifdef USE_FEMVIEW
    FemViewWindow* m_view;
#else
    FemWidget* m_view;
#endif
    std::vector<bool> m_selected;
    int m_currentItemIdx;
    NodeLoadPropPopupPtr m_propPopup;

public:
    NodeLoadsWindow(const std::string name);
    virtual ~NodeLoadsWindow();

    static std::shared_ptr<NodeLoadsWindow> create(const std::string name);

    void setFemNodeLoadSet(ofem::BeamNodeLoadSet* bcSet);
#ifdef USE_FEMVIEW
    void setFemView(FemViewWindow* view);
#else
    void setFemWidget(FemWidget* widget);
#endif

protected:
    virtual void doPreDraw();
    virtual void doDraw();
    virtual void doPostDraw();
};

typedef std::shared_ptr<NodeLoadsWindow> NodeLoadsWindowPtr;

}
