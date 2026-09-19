#include <ofui/self_weight_window.h>

#include <imgui.h>

#ifdef USE_FEMVIEW
#include <FemView.h>
#else
#include <FemWidget.h>
#endif

using namespace ofui;

SelfWeightWindow::SelfWeightWindow(const std::string name)
    : UiWindow(name), m_view{nullptr}, m_enabled{false}, m_mode{0}, m_gravity{9.81}, m_gravityScale{1.0},
      m_totalWeight{0.0}
{
}

SelfWeightWindow::~SelfWeightWindow()
{
}

std::shared_ptr<SelfWeightWindow> SelfWeightWindow::create(const std::string name)
{
    return std::make_shared<SelfWeightWindow>(name);
}

void SelfWeightWindow::setFemView(FemViewWindow *view)
{
    m_view = view;
}

void SelfWeightWindow::update()
{
    if (m_view == nullptr)
        return;

    m_enabled = m_view->selfWeightEnabled();
    m_mode = m_view->selfWeightMode();
    m_gravity = m_view->gravity();
    m_gravityScale = m_view->gravityScale();
    m_totalWeight = m_view->totalWeight();
}

// Every field applies immediately, like the rest of the app's settings panels
// (see SettingsWindow) -- there is no Apply button to remember to press.
// Checkbox/radio changes commit on click; numeric fields commit once the user
// is done editing (IsItemDeactivatedAfterEdit), not on every keystroke, so
// typing a value doesn't spam the undo stack with a snapshot per digit.
void SelfWeightWindow::doDraw()
{
    if (m_view == nullptr)
        return;

    if (ImGui::Checkbox("Enable self-weight", &m_enabled))
        m_view->setSelfWeightEnabled(m_enabled);

    ImGui::Separator();

    bool modeChanged = false;
    modeChanged |= ImGui::RadioButton("By material density", &m_mode, 0);
    ImGui::SameLine();
    modeChanged |= ImGui::RadioButton("Total load for structure", &m_mode, 1);
    if (modeChanged)
        m_view->setSelfWeightMode(m_mode);

    ImGui::Separator();

    if (m_mode == 0) {
        ImGui::InputDouble("Gravity (m/s^2)", &m_gravity, 0.0, 0.0, "%.6g");
        if (ImGui::IsItemDeactivatedAfterEdit())
            m_view->setGravity(m_gravity);
        ImGui::TextWrapped("Uses each material's density and cross-section area. Set density in Materials...");
    }
    else {
        ImGui::InputDouble("Total load (N)", &m_totalWeight, 0.0, 0.0, "%.6g");
        if (ImGui::IsItemDeactivatedAfterEdit())
            m_view->setTotalWeight(m_totalWeight);
        ImGui::TextWrapped("Spread evenly by length across all beams, regardless of material or section.");
    }

    ImGui::InputDouble("Load factor", &m_gravityScale, 0.0, 0.0, "%.6g");
    if (ImGui::IsItemDeactivatedAfterEdit())
        m_view->setGravityScale(m_gravityScale);

    ImGui::Separator();

    if (ImGui::Button("Close", ImVec2(120, 0))) {
        this->setVisible(false);
    }
}
