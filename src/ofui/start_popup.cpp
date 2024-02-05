#include <ofui/start_popup.h>

#include <imgui.h>
#include <imgui_internal.h>

using namespace ofui;

StartPopup::StartPopup(const std::string name, bool modal) : PopupWindow(name, modal), m_view(nullptr)
{
    this->setWindowFlags(ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);
    this->setSize(800, 800);
}

StartPopup::~StartPopup()
{}

std::shared_ptr<StartPopup> StartPopup::create(const std::string name, bool modal)
{
    return std::make_shared<StartPopup>(name, modal);
}

void ofui::StartPopup::setView(FemViewWindow *view)
{
    m_view = view;
}

void StartPopup::doPopup()
{
    // ImGui::Dummy(ImVec2(800.0, 800.0));
    ImGui::Text("Welcome to ObjectiveFrame!");

    ImGui::NewLine();

    if (ImGui::BeginTable("table2", 3))
    {
        for (int row = 0; row < 4; row++)
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("Row %d", row);
            ImGui::TableNextColumn();
            ImGui::Text("Some contents");
            ImGui::TableNextColumn();
            ImGui::Text("123.456");
        }
        ImGui::EndTable();
    }

    ImVec2 button_size = ImGui::CalcItemSize(ImVec2{120, 0}, 0.0f, 0.0f);
    ImVec2 winSize = ImGui::GetWindowSize();

    ImVec2 centre_position_for_button{
        // we have two buttons, so twice the size - and we need to account for the spacing in the middle
        (winSize.x - button_size.x - ImGui::GetStyle().ItemSpacing.x) / 2, (winSize.y - button_size.y) / 2};

    ImGui::SetCursorPosX(centre_position_for_button.x);
    if (ImGui::Button("OK", ImVec2(120, 0)))
    {
        this->close(PopupResult::OK);
        ImGui::CloseCurrentPopup();
    }
    ImGui::SetItemDefaultFocus();
}
