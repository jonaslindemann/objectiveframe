#include <ofui/element_prop_window.h>

#include <FemWidget.h>

using namespace ofui;

const double PI = 2 * acos(0.0);

ElementPropWindow::ElementPropWindow(const std::string name)
	:UiWindow(name),
	 m_beam{nullptr},
	 m_beamRotation{0.0f},
	 m_oldBeamRotation{0.0f},
	 m_widget{nullptr}
{

}

ElementPropWindow::~ElementPropWindow()
{
}

void ElementPropWindow::setBeam(vfem::Beam* beam)
{
	m_beam = beam;
	if (m_beam != nullptr)
	{
		m_beamRotation = float(m_beam->getBeam()->getBeamRotation()) * 2.0f * PI / 360.0f;
		m_oldBeamRotation = m_beamRotation; 
	}
}

void ElementPropWindow::setWidget(FemWidget* widget)
{
	m_widget = widget;
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
		ImGui::SliderAngle("Rotation", &m_beamRotation, -180.0f, 180.0f);

		if (m_beam->getBeam()->getValueSize() > 0)
		{
			if (ImGui::BeginTabBar("Test"))
			{
				if (ImGui::BeginTabItem("Section forces"))
				{
					int n = m_beam->getBeam()->getEvaluationPoints();
					int k = m_beam->getBeam()->getValueSize();

					int pos = 0;

					// N          Vy         Vz         T          My         Mz

					ImGui::BeginTable("SectForces", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::Text("N");
					ImGui::TableNextColumn();
					ImGui::Text("Vy");
					ImGui::TableNextColumn();
					ImGui::Text("Vz");
					ImGui::TableNextColumn();
					ImGui::Text("T");
					ImGui::TableNextColumn();
					ImGui::Text("My");
					ImGui::TableNextColumn();
					ImGui::Text("Mz");

					for (auto i = 0; i < n; i++)
					{
						ImGui::TableNextRow();
						for (auto j = 0; j < 6; j++)
						{
							ImGui::TableNextColumn();
							ImGui::Text("%.4f", m_beam->getBeam()->getValue(pos++));
						}
					}
					ImGui::EndTable();
					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Displacements"))
				{
					int n = m_beam->getBeam()->getEvaluationPoints();
					int k = m_beam->getBeam()->getValueSize();

					int pos = 6 * n;

					ImGui::BeginTable("Displ", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::Text("u");
					ImGui::TableNextColumn();
					ImGui::Text("w");
					ImGui::TableNextColumn();
					ImGui::Text("v");
					ImGui::TableNextColumn();
					ImGui::Text("phi");

					for (auto i = 0; i < n; i++)
					{
						ImGui::TableNextRow();
						for (auto j = 0; j < 4; j++)
						{
							ImGui::TableNextColumn();
							ImGui::Text("%.4f", m_beam->getBeam()->getValue(pos++));
						}
					}
					ImGui::EndTable();
					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}
	}
	else
	{
		ImGui::Text("Select a node to display node properties.");
	}

	if (m_oldBeamRotation!=m_beamRotation)
	{
		if (m_widget != nullptr)
		{
			m_widget->setRotationSelected(double(m_beamRotation * 360.0f / 2.0f / PI)); 
		}
	}
}
