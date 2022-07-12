#include "element_prop_window.h"

ElementPropWindow::ElementPropWindow(const std::string name)
	:UiWindow(name),
	 m_beam{nullptr}
{

}

ElementPropWindow::~ElementPropWindow()
{
}

void ElementPropWindow::setBeam(VisFemBeam* beam)
{
	m_beam = beam;
}

std::shared_ptr<ElementPropWindow> ElementPropWindow::create(const std::string name)
{
	return std::make_shared<ElementPropWindow>(name);
}

void ElementPropWindow::doDraw()
{
	ImGui::Dummy(ImVec2(150.0, 0.0));
	if (m_beam != nullptr)
	{
		ImGui::Text("Beam properties here");
	}
	else
	{
		ImGui::Text("Select a node to display node properties.");
	}
}
