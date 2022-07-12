#include "element_load_prop_popup.h"

#include <imgui.h>
#include <imgui_internal.h>

#include <FemWidget.h>

ElementLoadPropPopup::ElementLoadPropPopup(const std::string name, bool modal)
	:PopupWindow(name, modal),
	 m_widget{nullptr},
	 m_color{0},
	 m_nameArr{},
	 m_force{0.0, 0.0, 0.0}
{
}

ElementLoadPropPopup::~ElementLoadPropPopup()
{
}

std::shared_ptr<ElementLoadPropPopup> ElementLoadPropPopup::create(const std::string name, bool modal)
{
	return std::make_shared<ElementLoadPropPopup>(name, modal);
}

void ElementLoadPropPopup::setFemWidget(FemWidget* widget)
{
	m_widget = widget;
	this->update();
}

void ElementLoadPropPopup::update()
{
	auto load = m_widget->getCurrentBeamLoad();

	if (load != nullptr)
	{
		std::string name = load->getName();
		m_nameArr.fill(0);
		std::copy(name.begin(), name.end(), m_nameArr.data());

		m_color = load->getColor();

		double f[3];
		double value;
		
		//load->getDirection(f);
		load->getLocalDirection(f);
		value = load->getValue();

		m_force[0] = float(value * f[0]);
		m_force[1] = float(value * f[1]);
		m_force[2] = float(value * f[2]);
	}
}

void ElementLoadPropPopup::doPopup()
{
	if (m_widget != nullptr)
	{
		if (m_widget->getCurrentBeamLoad() != nullptr)
		{
			auto load = m_widget->getCurrentNodeLoad();
			ImGui::InputText("Name", m_nameArr.data(), 255);
			ImGui::InputInt("Color", &m_color);
			ImGui::Separator();

			ImGui::InputFloat3("Force", m_force);
		}
		else
		{
			ImGui::Text("Select a load first.");
		}
	}

	ImVec2 button_size = ImGui::CalcItemSize(ImVec2{ 120, 0 }, 0.0f, 0.0f);
	ImVec2 winSize = ImGui::GetWindowSize();

	ImVec2 centre_position_for_button{
		// we have two buttons, so twice the size - and we need to account for the spacing in the middle
		(winSize.x - button_size.x * 2 - ImGui::GetStyle().ItemSpacing.x) / 2,
		(winSize.y - button_size.y) / 2
	};

	ImGui::NewLine();

	ImGui::SetCursorPosX(centre_position_for_button.x);
	if (ImGui::Button("OK", ImVec2(120, 0))) 
	{ 
		this->close(PopupResult::OK);

		if (m_widget != nullptr)
		{
			auto load = m_widget->getCurrentNodeLoad();
			if (load != nullptr)
			{

				load->setName(m_nameArr.data());
				load->setColor(m_color);

				load->setValue(1.0);
				load->setDirection(m_force[0], m_force[1], m_force[2]);

				m_widget->set_changed();
			}
		}
		ImGui::CloseCurrentPopup(); 
	}
	ImGui::SetItemDefaultFocus();
	ImGui::SameLine();
	if (ImGui::Button("Cancel", ImVec2(120, 0))) 
	{ 
		this->close(PopupResult::CANCEL);
		ImGui::CloseCurrentPopup(); 
	}
}
