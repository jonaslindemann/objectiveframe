#include <ofui/settings_window.h>

#include "FemWidget.h"

using namespace ofui;

SettingsWindow::SettingsWindow(const std::string name)
	:UiWindow(name),
	m_size{ 20.0f },
	m_nodeSize{ 0.4f },
	m_lineRadius{ 0.15f },
	m_loadSize{ 7.0f },
	m_widget{ nullptr },
	m_scaleFactor{ 1.0f },
	m_lockScaleFactor{ false }
{

}

SettingsWindow::~SettingsWindow()
{
}

void SettingsWindow::setFemWidget(FemWidget* femWidget)
{
	m_widget = femWidget;
	m_size = float(m_widget->getWorkspace());
	m_nodeSize = float(m_widget->getRelNodeSize()*100.0f);
	m_loadSize = float(m_widget->getRelLoadSize()*100.0f);
	m_lineRadius = float(m_widget->getRelLineRadius()*100.0f);
	m_scaleFactor = float(m_widget->getScalefactor());
}

void SettingsWindow::update()
{
	m_size = float(m_widget->getWorkspace());
	m_nodeSize = float(m_widget->getRelNodeSize() * 100.0f);
	m_loadSize = float(m_widget->getRelLoadSize() * 100.0f);
	m_lineRadius = float(m_widget->getRelLineRadius() * 100.0f);
	m_scaleFactor = float(m_widget->getScalefactor());
}

std::shared_ptr<SettingsWindow> SettingsWindow::create(const std::string name)
{
	return std::make_shared<SettingsWindow>(name);
}

void SettingsWindow::doDraw()
{
	ImGui::Dummy(ImVec2(150.0, 0.0));

	m_size = std::nearbyint(m_size * 0.5f) * 2.0f;

	ImGui::SliderFloat("Size (units)", &m_size, 1.0f, 100.0f, "%.0f");
	ImGui::SliderFloat("Node size (%)", &m_nodeSize, 0.1f, 2.0f);
	ImGui::SliderFloat("Line radius (%)", &m_lineRadius, 0.1f, 2.0f);
	ImGui::SliderFloat("Load size (%)", &m_loadSize, 1.0f, 10.0f);

	ImGui::Separator();

	ImGui::SliderFloat("Scale factor", &m_scaleFactor, 1.0f, 1000.0f);
	ImGui::Checkbox("Lock scale factor", &m_lockScaleFactor);

	m_size = std::nearbyint(m_size * 0.5f) * 2.0f;

	if (m_widget != nullptr)
	{
		m_widget->setRelNodeSize(m_nodeSize/100.0f);
		m_widget->setRelLineRadius(m_lineRadius /100.0f);
		m_widget->setRelLoadSize(m_loadSize/100.0f);
		
		m_widget->setWorkspace(m_size, false);

		m_widget->setScalefactor(m_scaleFactor);

		if (m_lockScaleFactor)
			m_widget->lockScaleFactor();
		else
			m_widget->unlockScaleFactor();

	}
}
