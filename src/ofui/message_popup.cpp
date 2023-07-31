#include <ofui/message_popup.h>

#include <imgui.h>
#include <imgui_internal.h>

using namespace ofui;

MessagePopup::MessagePopup(const std::string name, bool modal) : PopupWindow(name, modal), m_message{"No message"}
{
}

MessagePopup::~MessagePopup()
{
}

std::shared_ptr<MessagePopup> MessagePopup::create(const std::string name, bool modal)
{
    return std::make_shared<MessagePopup>(name, modal);
}

void MessagePopup::setMessage(std::string message)
{
    m_message = message;
}

void MessagePopup::doPopup()
{
    ImGui::Text(m_message.c_str());

    ImVec2 button_size = ImGui::CalcItemSize(ImVec2{120, 0}, 0.0f, 0.0f);
    ImVec2 winSize = ImGui::GetWindowSize();

    ImVec2 centre_position_for_button{
        // we have two buttons, so twice the size - and we need to account for the spacing in the middle
        (winSize.x - button_size.x - ImGui::GetStyle().ItemSpacing.x) / 2, (winSize.y - button_size.y) / 2};

    ImGui::NewLine();

    ImGui::SetCursorPosX(centre_position_for_button.x);
    if (ImGui::Button("OK", ImVec2(120, 0))) {
        this->close(PopupResult::OK);
        ImGui::CloseCurrentPopup();
    }
    ImGui::SetItemDefaultFocus();
}
