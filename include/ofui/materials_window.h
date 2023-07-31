#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ofui/material_prop_popup.h>
#include <ofui/ui_window.h>

#include <ofem/beam_load_set.h>
#include <ofem/beam_material_set.h>

#ifdef USE_FEMVIEW
class FemViewWindow;
#else
class FemWidget;
#endif

namespace ofui {

class MaterialsWindow : public UiWindow {
private:
    ofem::BeamMaterialSet *m_materials;
#ifdef USE_FEMVIEW
    FemViewWindow *m_view;
#else
    FemWidget *m_view;
#endif
    std::vector<bool> m_selected;
    int m_currentItemIdx;
    MatetrialPropPopupPtr m_propPopup;

public:
    MaterialsWindow(const std::string name);
    virtual ~MaterialsWindow();

    static std::shared_ptr<MaterialsWindow> create(const std::string name);

    void setFemMaterialSet(ofem::BeamMaterialSet *materialSet);

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

typedef std::shared_ptr<MaterialsWindow> MaterialsWindowPtr;

} // namespace ofui
