#pragma once

#include <memory>
#include <string>
#include <vector>

#include <ofui/ui_window.h>
#include <ofui/bc_prop_popup.h>

#include <ofem/beam_node_bc_set.h>

class FemWidget;

namespace ofui {

class NodeBCsWindow : public UiWindow {
private:
	ofem::BeamNodeBCSet* m_femNodeBCSet;
	FemWidget* m_widget;
	std::vector<bool> m_selected;
	int m_currentItemIdx;
	BCPropPopupPtr m_propPopup;
public:
	NodeBCsWindow(const std::string name);
	virtual ~NodeBCsWindow();

	static std::shared_ptr<NodeBCsWindow> create(const std::string name);

	void setFemNodeBCSet(ofem::BeamNodeBCSet* bcSet);
	void setFemWidget(FemWidget* widget);
protected:
	virtual void doPreDraw();
	virtual void doDraw();
	virtual void doPostDraw();
};

typedef std::shared_ptr<NodeBCsWindow> NodeBCsWindowPtr;

}