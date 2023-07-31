#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ofui/element_load_prop_popup.h>
#include <ofui/ui_window.h>

#include <ofem/beam_load_set.h>

#ifdef USE_FEMVIEW
class FemViewWindow;
#else
class FemWidget;
#endif

namespace ofui {

class ElementLoadsWindow : public UiWindow {
private:
    ofem::BeamLoadSet *m_femBeamLoadSet;
#ifdef USE_FEMVIEW
    FemViewWindow *m_view;
#else
    FemWidget *m_view;
#endif
    std::vector<bool> m_selected;
    int m_currentItemIdx;
    ElementLoadPropPopupPtr m_propPopup;

public:
    ElementLoadsWindow(const std::string name);
    virtual ~ElementLoadsWindow();

    static std::shared_ptr<ElementLoadsWindow> create(const std::string name);

    void setFemLoadSet(ofem::BeamLoadSet *bcSet);
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

typedef std::shared_ptr<ElementLoadsWindow> ElementLoadsWindowPtr;

} // namespace ofui
