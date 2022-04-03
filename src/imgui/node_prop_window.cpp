#include "node_prop_window.h"

NodePropWindow::NodePropWindow(const std::string name)
	:UiWindow(name),
	 m_node{nullptr},
	 m_nodePos{0.0, 0.0, 0.0},
	m_nodeDispl{ 0.0, 0.0, 0.0 }

{

}

NodePropWindow::~NodePropWindow()
{
}

void NodePropWindow::setNode(VisFemNode* node)
{
	m_node = node;
}

std::shared_ptr<NodePropWindow> NodePropWindow::create(const std::string name)
{
	return std::make_shared<NodePropWindow>(name);
}

void NodePropWindow::doDraw()
{
	ImGui::Dummy(ImVec2(150.0, 0.0));
	if (m_node != nullptr)
	{
		double x, y, z;
		m_node->getPosition(x, y, z);
		m_nodePos[0] = float(x);
		m_nodePos[1] = float(y);
		m_nodePos[2] = float(z);
		ImGui::InputFloat3("Position", m_nodePos, "%.3f");
		m_node->setPosition(m_nodePos[0], m_nodePos[1], m_nodePos[2]);

		double dx, dy, dz;
		dx = m_node->getFemNode()->getValue(0);
		dy = m_node->getFemNode()->getValue(1);
		dz = m_node->getFemNode()->getValue(2);

		m_nodeDispl[0] = float(dx);
		m_nodeDispl[1] = float(dy);
		m_nodeDispl[2] = float(dz);
		ImGui::InputFloat3("Displacement", m_nodeDispl, "%.3f", ImGuiInputTextFlags_ReadOnly);
	}
	else
	{
		ImGui::Text("Select a node to display node properties.");
	}
}
