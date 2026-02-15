#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ofui/material_prop_popup.h>
#include <ofui/ui_window.h>

#include <ofem/beam_load_set.h>
#include <ofem/beam_material_set.h>

class FemViewWindow;

namespace ofui {

class MaterialsWindow : public UiWindow {
private:
    ofem::BeamMaterialSet *m_materials;
    FemViewWindow *m_view;
    std::vector<bool> m_selected;
    int m_currentItemIdx;
    MatetrialPropPopupPtr m_propPopup;

public:
    MaterialsWindow(const std::string name);
    virtual ~MaterialsWindow();

    static std::shared_ptr<MaterialsWindow> create(const std::string name);

    void setFemMaterialSet(ofem::BeamMaterialSet *materialSet);

    void setFemView(FemViewWindow *view);

protected:
    virtual void doPreDraw();
    virtual void doDraw();
    virtual void doPostDraw();
};

typedef std::shared_ptr<MaterialsWindow> MaterialsWindowPtr;

} // namespace ofui
