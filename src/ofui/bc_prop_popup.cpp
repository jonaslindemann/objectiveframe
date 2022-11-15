#include <ofui/bc_prop_popup.h>

#include <imgui.h>
#include <imgui_internal.h>

#ifdef USE_FEMVIEW
#include <FemView.h>
#else
#include <FemWidget.h>
#endif

using namespace ofui;

BCPropPopup::BCPropPopup(const std::string name, bool modal)
    : PopupWindow(name, modal)
    , m_view { nullptr }
    , m_color { 0 }
    , m_prescribedDofs { false, false, false, false, false, false }
    , m_prescribedValues { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 }
    , m_nameArr {}
{
}

BCPropPopup::~BCPropPopup()
{
}

std::shared_ptr<BCPropPopup> BCPropPopup::create(const std::string name, bool modal)
{
    return std::make_shared<BCPropPopup>(name, modal);
}

#ifdef USE_FEMVIEW
void BCPropPopup::setFemView(FemView* view)
{
    m_view = view;
    this->update();
}
#else
void BCPropPopup::setFemWidget(FemWidget* widget)
{
    m_view = widget;
    this->update();
}
#endif

void BCPropPopup::update()
{
    auto bc = m_view->getCurrentNodeBC();

    if (bc != nullptr)
    {
        std::string name = bc->getName();
        m_nameArr.fill(0);
        std::copy(name.begin(), name.end(), m_nameArr.data());

        m_prescribedDofs[0] = bc->isPrescribed(1);
        m_prescribedDofs[1] = bc->isPrescribed(2);
        m_prescribedDofs[2] = bc->isPrescribed(3);
        m_prescribedDofs[3] = bc->isPrescribed(4);
        m_prescribedDofs[4] = bc->isPrescribed(5);
        m_prescribedDofs[5] = bc->isPrescribed(6);
        m_prescribedValues[0] = bc->getPrescribedValue(1);
        m_prescribedValues[1] = bc->getPrescribedValue(2);
        m_prescribedValues[2] = bc->getPrescribedValue(3);
        m_prescribedValues[3] = bc->getPrescribedValue(4);
        m_prescribedValues[4] = bc->getPrescribedValue(5);
        m_prescribedValues[5] = bc->getPrescribedValue(6);
    }
}

void BCPropPopup::doPopup()
{
    if (m_view != nullptr)
    {
        if (m_view->getCurrentNodeBC() != nullptr)
        {
            auto bc = m_view->getCurrentNodeBC();
            ImGui::InputText("Name", m_nameArr.data(), 255);
            ImGui::InputInt("Color", &m_color);
            ImGui::Separator();

            ImGui::Checkbox("Fixed pos X", m_prescribedDofs);
            if (m_prescribedDofs[0])
                ImGui::InputDouble("Value X", m_prescribedValues);

            ImGui::Checkbox("Fixed pos Y", m_prescribedDofs + 1);
            if (m_prescribedDofs[1])
                ImGui::InputDouble("Value Y", m_prescribedValues + 1);

            ImGui::Checkbox("Fixed pos Z", m_prescribedDofs + 2);
            if (m_prescribedDofs[2])
                ImGui::InputDouble("Value Z", m_prescribedValues + 2);

            ImGui::Checkbox("Fixed rot X", m_prescribedDofs + 3);
            if (m_prescribedDofs[3])
                ImGui::InputDouble("Value rot X", m_prescribedValues + 3);

            ImGui::Checkbox("Fixed rot Y", m_prescribedDofs + 4);
            if (m_prescribedDofs[4])
                ImGui::InputDouble("Value rot Y", m_prescribedValues + 4);

            ImGui::Checkbox("Fixed rot Z", m_prescribedDofs + 5);
            if (m_prescribedDofs[5])
                ImGui::InputDouble("Value rot Z", m_prescribedValues + 5);
        }
        else
        {
            ImGui::Text("Select a BC first.");
        }
    }

    ImVec2 button_size = ImGui::CalcItemSize(ImVec2 { 120, 0 }, 0.0f, 0.0f);
    ImVec2 winSize = ImGui::GetWindowSize();

    ImVec2 centre_position_for_button {
        // we have two buttons, so twice the size - and we need to account for the spacing in the middle
        (winSize.x - button_size.x * 2 - ImGui::GetStyle().ItemSpacing.x) / 2,
        (winSize.y - button_size.y) / 2
    };

    ImGui::NewLine();

    ImGui::SetCursorPosX(centre_position_for_button.x);
    if (ImGui::Button("OK", ImVec2(120, 0)))
    {
        this->close(PopupResult::OK);

        if (m_view != nullptr)
        {
            auto bc = m_view->getCurrentNodeBC();
            if (bc != nullptr)
            {

                bc->setName(m_nameArr.data());
                bc->setColor(m_color);

                if (m_prescribedDofs[0])
                    bc->prescribe(1, m_prescribedValues[0]);
                else
                    bc->unprescribe(1);

                if (m_prescribedDofs[1])
                    bc->prescribe(2, m_prescribedValues[1]);
                else
                    bc->unprescribe(2);

                if (m_prescribedDofs[2])
                    bc->prescribe(3, m_prescribedValues[2]);
                else
                    bc->unprescribe(3);

                if (m_prescribedDofs[3])
                    bc->prescribe(4, m_prescribedValues[3]);
                else
                    bc->unprescribe(4);

                if (m_prescribedDofs[4])
                    bc->prescribe(5, m_prescribedValues[4]);
                else
                    bc->unprescribe(5);

                if (m_prescribedDofs[5])
                    bc->prescribe(6, m_prescribedValues[5]);
                else
                    bc->unprescribe(6);

                m_view->set_changed();
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
