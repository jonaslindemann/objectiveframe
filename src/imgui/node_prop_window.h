#pragma once

#include "ui_window.h"

#include <VisFemNode.h>

class NodePropWindow : public UiWindow {
private:
	std::string m_name;
	VisFemNode* m_node;
	float m_nodePos[3];
	float m_nodeDispl[3];
public:
	NodePropWindow(const std::string name);
	virtual ~NodePropWindow();

	void setNode(VisFemNode* node);

	static std::shared_ptr<NodePropWindow> create(const std::string name);

protected:
	virtual void doDraw();
};

typedef std::shared_ptr<NodePropWindow> NodePropWindowPtr;

