#include <ofui/color_scale_window.h>

#ifdef USE_FEMVIEW
#include <FemView.h>
#else
#include <FemWidget.h>
#endif

#include <ofui/ui_profile.h>
#include <vfem/beam_model.h>

using namespace ofui;

ColorScaleWindow::ColorScaleWindow(const std::string name) : UiWindow(name), m_view{nullptr}
{}

ColorScaleWindow::~ColorScaleWindow()
{}

void ofui::ColorScaleWindow::setView(FemViewWindow *view)
{
    m_view = view;
}

std::shared_ptr<ColorScaleWindow> ColorScaleWindow::create(const std::string name)
{
    return std::make_shared<ColorScaleWindow>(name);
}

void ColorScaleWindow::doDraw()
{
    // Width pinned to the font size: the panel auto-resizes, so wrapped text
    // would otherwise wrap to last frame's width, and tabs of different widths
    // would make it jump sideways.
    const float panelWidth = ImGui::GetFontSize() * 18.0f;
    ImGui::Dummy(ImVec2(panelWidth, 0.0f));

    if (m_view == nullptr)
        return;

    auto beamModel = static_cast<vfem::BeamModel *>(m_view->getModel());
    auto &scale = beamModel->colorScale();

    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + panelWidth);
    ImGui::TextUnformatted("Colors used for the beam result color scales. Changes apply immediately.");
    ImGui::PopTextWrapPos();

    // One tab per result type. A bar profile only shows normal force, so the
    // scales for the other results are hidden along with the results.
    const bool beamResults = UiProfile::instance()->has(UiFeature::BeamResultTypes);

    if (ImGui::BeginTabBar("##color_scale_tabs"))
    {
        if (ImGui::BeginTabItem("Axial force"))
        {
            ImGui::SeparatorText("Tension (+)");
            ImGui::ColorEdit3("Low##axialPos0", scale.axialPos[0]);
            ImGui::ColorEdit3("Mid-low##axialPos1", scale.axialPos[1]);
            ImGui::ColorEdit3("Mid-high##axialPos2", scale.axialPos[2]);
            ImGui::ColorEdit3("High##axialPos3", scale.axialPos[3]);

            ImGui::SeparatorText("Compression (-)");
            ImGui::ColorEdit3("Low##axialNeg0", scale.axialNeg[0]);
            ImGui::ColorEdit3("Mid-low##axialNeg1", scale.axialNeg[1]);
            ImGui::ColorEdit3("Mid-high##axialNeg2", scale.axialNeg[2]);
            ImGui::ColorEdit3("High##axialNeg3", scale.axialNeg[3]);

            ImGui::EndTabItem();
        }

        if (beamResults && ImGui::BeginTabItem("Moment / shear"))
        {
            ImGui::SeparatorText("Torsion, shear and moment");
            ImGui::ColorEdit3("Low##magnitude0", scale.magnitude[0]);
            ImGui::ColorEdit3("Mid##magnitude1", scale.magnitude[1]);
            ImGui::ColorEdit3("High##magnitude2", scale.magnitude[2]);

            ImGui::EndTabItem();
        }

        if (beamResults && ImGui::BeginTabItem("Navier"))
        {
            ImGui::SeparatorText("Utilization");
            ImGui::ColorEdit3("Low##navier0", scale.navier[0]);
            ImGui::ColorEdit3("Mid##navier1", scale.navier[1]);
            ImGui::ColorEdit3("High##navier2", scale.navier[2]);
            ImGui::ColorEdit3("Overload (>100%)##navierOverload", scale.navierOverload);

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::Separator();

    // Resets every scale, not only the one in the open tab.
    if (ImGui::Button("Reset all to defaults"))
        beamModel->resetColorScaleToDefaults();
}
