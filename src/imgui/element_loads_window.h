#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ui_window.h>
#include <FemBeamLoadSet.h>

#include <element_load_prop_popup.h>

class FemWidget;

class ElementLoadsWindow : public UiWindow {
private:
	CFemBeamLoadSet* m_femBeamLoadSet;
	FemWidget* m_widget;
	std::vector<bool> m_selected;
	int m_currentItemIdx;
	ElementLoadPropPopupPtr m_propPopup;
public:
	ElementLoadsWindow(const std::string name);
	virtual ~ElementLoadsWindow();

	static std::shared_ptr<ElementLoadsWindow> create(const std::string name);

	void setFemLoadSet(CFemBeamLoadSet* bcSet);
	void setFemWidget(FemWidget* widget);
protected:
	virtual void doPreDraw();
	virtual void doDraw();
	virtual void doPostDraw();
};

typedef std::shared_ptr<ElementLoadsWindow> ElementLoadsWindowPtr;


