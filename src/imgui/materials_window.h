#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ui_window.h>
#include <FemBeamLoadSet.h>
#include <FemBeamMaterialSet.h>

#include <material_prop_popup.h>

class FemWidget;

class MaterialsWindow : public UiWindow {
private:
	CFemBeamMaterialSet* m_materials;
	FemWidget* m_widget;
	std::vector<bool> m_selected;
	int m_currentItemIdx;
	MatetrialPropPopupPtr m_propPopup;
public:
	MaterialsWindow(const std::string name);
	virtual ~MaterialsWindow();

	static std::shared_ptr<MaterialsWindow> create(const std::string name);

	void setFemMaterialSet(CFemBeamMaterialSet* materialSet);
	void setFemWidget(FemWidget* widget);
protected:
	virtual void doPreDraw();
	virtual void doDraw();
	virtual void doPostDraw();
};

 typedef std::shared_ptr<MaterialsWindow> MaterialsWindowPtr;


