#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ui_window.h>
#include <FemBeamNodeLoadSet.h>

#include <node_load_prop_popup.h>

class FemWidget;

class NodeLoadsWindow : public UiWindow {
private:
	CFemBeamNodeLoadSet* m_femNodeLoadSet;
	FemWidget* m_widget;
	std::vector<bool> m_selected;
	int m_currentItemIdx;
	NodeLoadPropPopupPtr m_propPopup;
public:
	NodeLoadsWindow(const std::string name);
	virtual ~NodeLoadsWindow();

	static std::shared_ptr<NodeLoadsWindow> create(const std::string name);

	void setFemNodeLoadSet(CFemBeamNodeLoadSet* bcSet);
	void setFemWidget(FemWidget* widget);
protected:
	virtual void doPreDraw();
	virtual void doDraw();
	virtual void doPostDraw();
};

typedef std::shared_ptr<NodeLoadsWindow> NodeLoadsWindowPtr;


