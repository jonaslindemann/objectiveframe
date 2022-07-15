#include <ofui/new_model_popup.h>

#include <imgui.h>
#include <imgui_internal.h>

using namespace ofui;

NewModelPopup::NewModelPopup(const std::string name, bool modal)
	:PopupWindow(name, modal),
	 m_lineRadius{0.15f},
	 m_modelSize{20.0f},
	 m_nodeSize{0.5f},
	 m_loadSize{7.0f}
{
}

NewModelPopup::~NewModelPopup()
{
}

std::shared_ptr<NewModelPopup> NewModelPopup::create(const std::string name, bool modal)
{
	return std::make_shared<NewModelPopup>(name, modal);
}

float NewModelPopup::modelSize()
{
	return m_modelSize;
}

float NewModelPopup::nodeSize()
{
	return m_nodeSize;
}

float NewModelPopup::lineRadius()
{
	return m_lineRadius;
}

float NewModelPopup::loadSize()
{
	return m_loadSize;
}

void NewModelPopup::doPopup()
{
	ImGui::DragFloat("Size (units)", &m_modelSize, 1.0f, 5.0f, 100.0f, "%.3f");
	ImGui::DragFloat("Node size (% workspace)", &m_nodeSize, 0.1f, 0.5f, 5.0f, "%.3f");
	ImGui::DragFloat("Line radius (% workspace)", &m_lineRadius, 0.1f, 0.5f, 5.0f, "%.3f");
	ImGui::DragFloat("Load size (% workspace)", &m_loadSize, 0.1f, 0.5f, 5.0f, "%.3f");
	/*DragFloat
	ImGui::InputFloat("Size (units)", &m_modelSize, 1.0f, 5.0f, "%.3f");
	ImGui::InputFloat("Node size (% workspace)", &m_nodeSize, 0.05f, 0.5f, "%.3f");
	ImGui::InputFloat("Line radius (% workspace)", &m_lineRadius, 0.05, 0.5f, "%.3f");
	ImGui::InputFloat("Load size (% workspace)", &m_loadSize, 0.05f, 0.5f, "%.3f");
	*/

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
