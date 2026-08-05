#include <ofui/color_scale_window.h>

#ifdef USE_FEMVIEW
#include <FemView.h>
#else
#include <FemWidget.h>
#endif

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
    ImGui::Dummy(ImVec2(280.0, 0.0));

    if (m_view == nullptr)
        return;

    auto beamModel = static_cast<vfem::BeamModel *>(m_view->getModel());
    auto &scale = beamModel->colorScale();

    ImGui::TextWrapped("Colors used for the beam result color scales. Changes apply immediately.");
    ImGui::Separator();

    if (ImGui::CollapsingHeader("Axial force (N)", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("Tension (+)");
        ImGui::ColorEdit3("Low##axialPos0", scale.axialPos[0]);
        ImGui::ColorEdit3("Mid-low##axialPos1", scale.axialPos[1]);
        ImGui::ColorEdit3("Mid-high##axialPos2", scale.axialPos[2]);
        ImGui::ColorEdit3("High##axialPos3", scale.axialPos[3]);

        ImGui::Spacing();

        ImGui::Text("Compression (-)");
        ImGui::ColorEdit3("Low##axialNeg0", scale.axialNeg[0]);
        ImGui::ColorEdit3("Mid-low##axialNeg1", scale.axialNeg[1]);
        ImGui::ColorEdit3("Mid-high##axialNeg2", scale.axialNeg[2]);
        ImGui::ColorEdit3("High##axialNeg3", scale.axialNeg[3]);
    }

    if (ImGui::CollapsingHeader("Torsion / Shear / Moment", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::ColorEdit3("Low##magnitude0", scale.magnitude[0]);
        ImGui::ColorEdit3("Mid##magnitude1", scale.magnitude[1]);
        ImGui::ColorEdit3("High##magnitude2", scale.magnitude[2]);
    }

    if (ImGui::CollapsingHeader("Navier utilization", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::ColorEdit3("Low##navier0", scale.navier[0]);
        ImGui::ColorEdit3("Mid##navier1", scale.navier[1]);
        ImGui::ColorEdit3("High##navier2", scale.navier[2]);
        ImGui::ColorEdit3("Overload (>100%)##navierOverload", scale.navierOverload);
    }

    ImGui::Separator();

    if (ImGui::Button("Reset to defaults"))
        beamModel->resetColorScaleToDefaults();
}
