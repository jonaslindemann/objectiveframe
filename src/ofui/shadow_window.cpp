#include <ofui/shadow_window.h>

#ifdef USE_FEMVIEW
#include <FemView.h>
#else
#include <FemWidget.h>
#endif

using namespace ofui;

namespace {

// Offered resolutions, smallest first. 512 is there for a slow machine rather
// than because it looks good; 4096 is 64 MB of depth texture and only pays off
// on a very large model.
const int kMapSizes[] = {512, 1024, 2048, 4096};
const char *kMapSizeLabels[] = {"512", "1024", "2048", "4096"};
const int kMapSizeCount = 4;

int indexForMapSize(int size)
{
    for (int i = 0; i < kMapSizeCount; i++)
        if (kMapSizes[i] == size)
            return i;

    return 2; // 2048, the default
}

} // namespace

ShadowWindow::ShadowWindow(const std::string name)
    : UiWindow(name), m_useShadows{true}, m_azimuth{52.0f}, m_elevation{60.0f}, m_strength{0.45f}, m_mapSizeIndex{2},
      m_view{nullptr}
{}

ShadowWindow::~ShadowWindow()
{}

void ShadowWindow::setView(FemViewWindow *view)
{
    m_view = view;
    this->update();
}

std::shared_ptr<ShadowWindow> ShadowWindow::create(const std::string name)
{
    return std::make_shared<ShadowWindow>(name);
}

void ShadowWindow::update()
{
    if (m_view == nullptr)
        return;

    m_useShadows = m_view->getUseShadows();
    m_azimuth = float(m_view->getShadowAzimuth());
    m_elevation = float(m_view->getShadowElevation());
    m_strength = float(m_view->getShadowStrength());
    m_mapSizeIndex = indexForMapSize(m_view->getShadowMapSize());
}

void ShadowWindow::doDraw()
{
    if (m_view == nullptr)
    {
        ImGui::TextDisabled("No view assigned.");
        return;
    }

    // Read the live values back every frame rather than trusting the local
    // copies: the same settings are reachable from the View menu and from Alt+4,
    // so the panel is not the only thing that can change them.

    this->update();

    ImGui::Dummy(ImVec2(260.0f, 0.0f));

    const bool xray = m_view->getUseBlending();

    ImGui::BeginDisabled(xray);

    if (ImGui::Checkbox("Enable shadows", &m_useShadows))
        m_view->setUseShadows(m_useShadows);

    ImGui::EndDisabled();

    if (xray)
    {
        ImGui::SameLine();
        ImGui::TextDisabled("(off in X-ray mode)");
    }

    ImGui::Separator();

    ImGui::TextUnformatted("Light direction");

    // Every control below writes only when it actually changed. Each write
    // rebuilds the shadow map, and this panel is redrawn every frame.

    if (ImGui::SliderFloat("Azimuth", &m_azimuth, 0.0f, 360.0f, "%.0f deg"))
        m_view->setShadowAzimuth(m_azimuth);

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Compass bearing the light comes from.");

    if (ImGui::SliderFloat("Elevation", &m_elevation, 5.0f, 89.0f, "%.0f deg"))
        m_view->setShadowElevation(m_elevation);

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Height of the light above the horizon. A low light casts long shadows.");

    ImGui::Separator();

    ImGui::TextUnformatted("Appearance");

    if (ImGui::SliderFloat("Intensity", &m_strength, 0.0f, 1.0f, "%.2f"))
        m_view->setShadowStrength(m_strength);

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("How dark a shadowed surface goes. Ambient light is never removed, so even 1.0 "
                          "leaves the surface readable.");

    if (ImGui::Combo("Resolution", &m_mapSizeIndex, kMapSizeLabels, kMapSizeCount))
        m_view->setShadowMapSize(kMapSizes[m_mapSizeIndex]);

    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Shadow map size in texels. Higher is sharper and costs memory, but not frame rate: "
                          "the map is only redrawn when the model changes.");

    ImGui::Separator();

    if (ImGui::Button("Reset to defaults"))
        m_view->resetShadowDefaults();
}
