#include <ofui/self_weight_window.h>

#include <imgui.h>

#ifdef USE_FEMVIEW
#include <FemView.h>
#else
#include <FemWidget.h>
#endif

using namespace ofui;

namespace {

// ofem::SelfWeightMode as plain ints -- ofui headers must not include objframe
// or ofem headers, and FemViewWindow exchanges the mode as an int anyway.
constexpr int ModeDensity = 0;
constexpr int ModeTotalLoad = 1;
constexpr int ModeMassPerLength = 2;

} // namespace

SelfWeightWindow::SelfWeightWindow(const std::string name)
    : UiWindow(name), m_view{nullptr}, m_enabled{false}, m_mode{ModeDensity}, m_gravity{9.81}, m_gravityScale{1.0},
      m_totalWeight{0.0}, m_massPerLength{0.0}, m_syncTab{true}
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
    m_massPerLength = m_view->massPerLength();

    // The mode just came from the model, so the tab bar has to be moved onto it
    // once; after that the user's clicks own the selection.
    m_syncTab = true;
}

ImGuiTabItemFlags SelfWeightWindow::tabFlags(int mode)
{
    return (m_syncTab && m_mode == mode) ? ImGuiTabItemFlags_SetSelected : 0;
}

// Called from the body of each tab: a tab is only drawn while it is the active
// one, so being drawn is what selects the mode. On the frame update() forced the
// selection the mode already matches and nothing is written.
void SelfWeightWindow::selectMode(int mode)
{
    if (m_mode == mode)
        return;

    m_mode = mode;
    m_view->setSelfWeightMode(mode);
}

// Shared by the two tabs that turn a mass into a force.
void SelfWeightWindow::drawGravityField()
{
    ImGui::InputDouble("Gravity (m/s^2)", &m_gravity, 0.0, 0.0, "%.6g");
    if (ImGui::IsItemDeactivatedAfterEdit())
        m_view->setGravity(m_gravity);
}

// Every field applies immediately, like the rest of the app's settings panels
// (see SettingsWindow) -- there is no Apply button to remember to press.
// Checkbox/tab changes commit on click; numeric fields commit once the user
// is done editing (IsItemDeactivatedAfterEdit), not on every keystroke, so
// typing a value doesn't spam the undo stack with a snapshot per digit.
void SelfWeightWindow::doDraw()
{
    if (m_view == nullptr)
        return;

    if (ImGui::Checkbox("Enable self-weight", &m_enabled))
        m_view->setSelfWeightEnabled(m_enabled);

    ImGui::Separator();

    if (ImGui::BeginTabBar("self_weight_modes"))
    {
        if (ImGui::BeginTabItem("Material density", nullptr, tabFlags(ModeDensity)))
        {
            selectMode(ModeDensity);
            drawGravityField();
            ImGui::TextWrapped("Uses each material's density and cross-section area. Set density in Materials...");
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Mass per length", nullptr, tabFlags(ModeMassPerLength)))
        {
            selectMode(ModeMassPerLength);
            ImGui::InputDouble("Mass per length (kg/m)", &m_massPerLength, 0.0, 0.0, "%.6g");
            if (ImGui::IsItemDeactivatedAfterEdit())
                m_view->setMassPerLength(m_massPerLength);
            drawGravityField();
            ImGui::TextWrapped("Same mass per unit length on every beam, regardless of material or section.");
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Total load", nullptr, tabFlags(ModeTotalLoad)))
        {
            selectMode(ModeTotalLoad);
            ImGui::InputDouble("Total load (N)", &m_totalWeight, 0.0, 0.0, "%.6g");
            if (ImGui::IsItemDeactivatedAfterEdit())
                m_view->setTotalWeight(m_totalWeight);
            ImGui::TextWrapped("Spread evenly by length across all beams, regardless of material or section.");
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    m_syncTab = false;

    ImGui::Separator();

    ImGui::InputDouble("Load factor", &m_gravityScale, 0.0, 0.0, "%.6g");
    if (ImGui::IsItemDeactivatedAfterEdit())
        m_view->setGravityScale(m_gravityScale);

    ImGui::Separator();

    if (ImGui::Button("Close", ImVec2(120, 0))) {
        this->setVisible(false);
    }
}
