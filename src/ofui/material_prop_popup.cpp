#include <ofui/material_prop_popup.h>

#include <imgui.h>
#include <imgui_internal.h>

#ifdef USE_FEMVIEW
#include <FemView.h>
#else
#include <FemWidget.h>
#endif

using namespace ofem;
using namespace ofui;

MaterialPropPopup::MaterialPropPopup(const std::string name, bool modal)
    : PopupWindow(name, modal), m_view{nullptr}, m_color{0}, m_nameArr{}, m_E{0.0}, m_G{0.0}, m_A{0.0}, m_Iy{0.0},
      m_Iz{0.0}, m_Kv{0.0}, m_section{0}, m_height{0.0}, m_width{0.0}, m_ufw{0.0}, m_lfw{0.0}, m_wt{0.0}, m_uft{0.0},
      m_lft{0.0}, m_ulfw{0.0}, m_llfw{0.0}, m_outerRadius{0.0}, m_innerRadius{0.0}, m_oldSection{0}
{
}

MaterialPropPopup::~MaterialPropPopup()
{
}

std::shared_ptr<MaterialPropPopup> MaterialPropPopup::create(const std::string name, bool modal)
{
    return std::make_shared<MaterialPropPopup>(name, modal);
}

#ifdef USE_FEMVIEW
void ofui::MaterialPropPopup::setFemView(FemViewWindow *view)
{
    m_view = view;
}
#else
void MaterialPropPopup::setFemWidget(FemWidget *widget)
{
    m_view = widget;
    this->update();
}
#endif

void MaterialPropPopup::update()
{
    auto material = m_view->getCurrentMaterial();

    if (material != nullptr) {
        std::string name = material->getName();
        m_nameArr.fill(0);
        std::copy(name.begin(), name.end(), m_nameArr.data());

        m_color = material->getColor();

        auto section = material->getSection();

        m_section = section->getSectionType();

        /*
        m_prop[0]  = height
        m_prop[1]  = width
        m_prop[2]  = UFW	(Upper Flange Width)
        m_prop[3]  = LFW	(Lower Flange Width)
        m_prop[4]  = WT		(Waist Thickness)
        m_prop[5]  = UFT	(Upper Flange Thickness)
        m_prop[6]  = LFT	(Lower Flange Thickness)
        m_prop[7]  = ULFW	(Upper Left Flange Width)
        m_prop[8]  = LLFW	(Lower Left Flange Width)
        m_prop[9]  = OuterRadius
        m_prop[10] = InnerRadius
        */

        double *props;
        section->getAllProps(props);

        m_width = props[0];
        m_height = props[1];
        m_ufw = props[2];
        m_lfw = props[3];
        m_wt = props[4];
        m_uft = props[5];
        m_lft = props[6];
        m_ulfw = props[7];
        m_llfw = props[8];
        m_outerRadius = props[9];
        m_innerRadius = props[10];

        updateMaterial();

        material->getProperties(m_E, m_G, m_A, m_Iy, m_Iz, m_Kv);
    }
}

void MaterialPropPopup::updateMaterial()
{
    auto material = m_view->getCurrentMaterial();

    auto section = material->getSection();

    if (material != nullptr) {
        if (m_section != m_oldSection) {
            material->setSectionType(static_cast<SectionType>(m_section));
            m_oldSection = m_section;
        }
        section = material->getSection();

        section->setSectionProps(m_width, m_height, m_ufw, m_lfw, m_wt, m_uft, m_lft, m_ulfw, m_llfw, m_outerRadius,
                                 m_innerRadius);
        section->calcDataFromSection();

        m_E = section->E();
        m_G = section->G();
        m_A = section->A();
        m_Iz = section->Iz();
        m_Iy = section->Iy();
        m_Kv = section->Kv();
    }
}

void MaterialPropPopup::doPopup()
{
    BeamMaterial *material = nullptr;
    Section *section = nullptr;

    if (m_view != nullptr) {
        if (m_view->getCurrentMaterial() != nullptr) {
            material = m_view->getCurrentMaterial();
            section = material->getSection();

            if (ImGui::BeginTabBar("Test")) {
                if (ImGui::BeginTabItem("General")) {
                    ImGui::InputText("Name", m_nameArr.data(), 255);
                    ImGui::SliderInt("Color", &m_color, 0, 255);
                    ImGui::SameLine();

                    const ImVec2 posR = ImGui::GetCursorScreenPos();

                    auto beamModel = m_view->getVisualBeamModel();
                    auto colorTable = beamModel->getColorTable();

                    float r, g, b;
                    colorTable->getColor(m_color, r, g, b);
                    auto drawList = ImGui::GetWindowDrawList();
                    ImColor col(r, g, b);
                    drawList->AddRectFilled(posR, ImVec2(posR.x + 200, posR.y + ImGui::GetFrameHeight()), col, 5.0f,
                                            ImDrawFlags_RoundCornersAll);
                    ImGui::SetCursorScreenPos(ImVec2(posR.x + 200, posR.y + ImGui::GetFrameHeightWithSpacing()));

                    ImGui::Separator();

                    ImGui::InputDouble("Young's modulus", &m_E, 0.0, 0.0, "%.6g");
                    ImGui::InputDouble("Shear modulus", &m_G, 0.0, 0.0, "%.6g");

                    ImGui::Separator();

                    ImGui::InputDouble("Area", &m_A, 0.0, 0.0, "%.6g");
                    ImGui::InputDouble("Moment of inertia (y)", &m_Iy, 0.0, 0.0, "%.6g");
                    ImGui::InputDouble("Moment of inertia (z)", &m_Iz, 0.0, 0.0, "%.6g");
                    ImGui::InputDouble("Saint-Venant const", &m_Kv, 0.0, 0.0, "%.6g");

                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Section")) {
                    ImGui::RadioButton("Rectangular", &m_section, static_cast<SectionType>(ST_Rectangle));
                    ImGui::RadioButton("Hollow rectangular", &m_section, static_cast<SectionType>(ST_RHS));
                    ImGui::RadioButton("I-section", &m_section, static_cast<SectionType>(ST_I));
                    ImGui::RadioButton("U-section", &m_section, static_cast<SectionType>(ST_U));
                    ImGui::RadioButton("L-section", &m_section, static_cast<SectionType>(ST_L));
                    ImGui::RadioButton("Circular", &m_section, static_cast<SectionType>(ST_SolidPipe));
                    ImGui::RadioButton("Hollow circular", &m_section, static_cast<SectionType>(ST_Pipe));

                    ImGui::Separator();

                    if (m_section == static_cast<SectionType>(ST_Rectangle)) {
                        ImGui::InputDouble("Width", &m_width, 0.0, 0.0, "%.6g");
                        ImGui::InputDouble("Height", &m_height, 0.0, 0.0, "%.6g");
                    }
                    if (m_section == static_cast<SectionType>(ST_RHS)) {
                        ImGui::InputDouble("Width", &m_width, 0.0, 0.0, "%.6g");
                        ImGui::InputDouble("Height", &m_height, 0.0, 0.0, "%.6g");
                        ImGui::InputDouble("Waiste thickness", &m_wt, 0.0, 0.0, "%.6g");
                    }
                    if (m_section == static_cast<SectionType>(ST_I)) {
                        ImGui::InputDouble("Width", &m_width, 0.0, 0.0, "%.6g");
                        ImGui::InputDouble("Height", &m_height, 0.0, 0.0, "%.6g");
                        ImGui::InputDouble("Upper flange thickness", &m_uft, 0.0, 0.0, "%.6g");
                        ImGui::InputDouble("Lower flange thickness", &m_lft, 0.0, 0.0, "%.6g");
                        ImGui::InputDouble("Upper flange width", &m_ufw, 0.0, 0.0, "%.6g");
                        ImGui::InputDouble("Lower flange width", &m_lfw, 0.0, 0.0, "%.6g");
                        ImGui::InputDouble("Waiste thickness", &m_wt, 0.0, 0.0, "%.6g");
                    }
                    if (m_section == static_cast<SectionType>(ST_U)) {
                        ImGui::InputDouble("Width", &m_width, 0.0, 0.0, "%.6g");
                        ImGui::InputDouble("Height", &m_height, 0.0, 0.0, "%.6g");
                        ImGui::InputDouble("Upper flange thickness", &m_uft, 0.0, 0.0, "%.6g");
                        ImGui::InputDouble("Lower flange thickness", &m_lft, 0.0, 0.0, "%.6g");
                        ImGui::InputDouble("Waiste thickness", &m_wt, 0.0, 0.0, "%.6g");
                    }
                    if (m_section == static_cast<SectionType>(ST_L)) {
                        ImGui::InputDouble("Width", &m_width, 0.0, 0.0, "%.6g");
                        ImGui::InputDouble("Height", &m_height, 0.0, 0.0, "%.6g");
                        ImGui::InputDouble("Upper flange thickness", &m_uft, 0.0, 0.0, "%.6g");
                        ImGui::InputDouble("Waiste thickness", &m_wt, 0.0, 0.0, "%.6g");
                    }
                    if (m_section == static_cast<SectionType>(ST_SolidPipe)) {
                        ImGui::InputDouble("Outer radius", &m_outerRadius, 0.0, 0.0, "%.6g");
                    }
                    if (m_section == static_cast<SectionType>(ST_Pipe)) {
                        ImGui::InputDouble("Outer radius", &m_outerRadius, 0.0, 0.0, "%.6g");
                        ImGui::InputDouble("Inner radius", &m_innerRadius, 0.0, 0.0, "%.6g");
                    }

                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
        }
        else {
            ImGui::Text("Select a load first.");
        }
    }

    updateMaterial();

    ImVec2 button_size = ImGui::CalcItemSize(ImVec2{120, 0}, 0.0f, 0.0f);
    ImVec2 winSize = ImGui::GetWindowSize();

    ImVec2 centre_position_for_button{
        // we have two buttons, so twice the size - and we need to account for the spacing in the middle
        (winSize.x - button_size.x * 2 - ImGui::GetStyle().ItemSpacing.x) / 2, (winSize.y - button_size.y) / 2};

    ImGui::NewLine();

    ImGui::SetCursorPosX(centre_position_for_button.x);
    if (ImGui::Button("OK", ImVec2(120, 0))) {
        this->close(PopupResult::OK);

        if (m_view != nullptr) {
            auto material = m_view->getCurrentMaterial();
            if (material != nullptr) {

                material->setName(m_nameArr.data());
                material->setColor(m_color);
                material->assignPropFromSection();

                m_view->set_changed();
                m_view->redraw();
            }
        }
        ImGui::CloseCurrentPopup();
    }
    ImGui::SetItemDefaultFocus();
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(120, 0))) {
        this->close(PopupResult::CANCEL);
        ImGui::CloseCurrentPopup();
    }
}
