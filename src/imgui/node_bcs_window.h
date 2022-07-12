#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ui_window.h>
#include <FemBeamNodeBCSet.h>

#include <bc_prop_popup.h>

class FemWidget;

class NodeBCsWindow : public UiWindow {
private:
	CFemBeamNodeBCSet* m_femNodeBCSet;
	FemWidget* m_widget;
	std::vector<bool> m_selected;
	int m_currentItemIdx;
	BCPropPopupPtr m_propPopup;
public:
	NodeBCsWindow(const std::string name);
	virtual ~NodeBCsWindow();

	static std::shared_ptr<NodeBCsWindow> create(const std::string name);

	void setFemNodeBCSet(CFemBeamNodeBCSet* bcSet);
	void setFemWidget(FemWidget* widget);
protected:
	virtual void doPreDraw();
	virtual void doDraw();
	virtual void doPostDraw();
};

typedef std::shared_ptr<NodeBCsWindow> NodeBCsWindowPtr;


