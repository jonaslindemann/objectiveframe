#include <ofui/eigenmode_window.h>

#include <imgui.h>
#include <logger.h>
#include <ofui/ui_profile.h>

#include <FemView.h>

#include <cmath>
#include <string>

using namespace ofui;

namespace {

// Two labels over one ImGui identity. Everything after "###" is the id, so the
// panel can be renamed when the profile changes without ImGui deciding it is a
// different window and dropping the position and size the user gave it. Must
// match the id the window is created with in FemViewWindow::onInit().

const char *advancedTitle = "Eigenmode Analysis###eigenmodeWindow";
const char *simpleTitle = "Stability analysis###eigenmodeWindow";

const ImVec4 unstableColor(1.0f, 0.35f, 0.35f, 1.0f);

// Rows shown before the mode table starts to scroll.

const int maxVisibleRows = 8;

// "UNSTABLE" for a negative eigenvalue, otherwise the frequency it stands for.

std::string modeDescription(double lambda)
{
    if (lambda < 0.0)
        return "UNSTABLE";

    char buf[64];
    std::snprintf(buf, sizeof(buf), "%.3f Hz", std::sqrt(lambda) / (2.0 * 3.14159265358979323846));
    return buf;
}

void descriptionText(double lambda)
{
    const std::string text = modeDescription(lambda);
    if (lambda < 0.0)
        ImGui::TextColored(unstableColor, "%s", text.c_str());
    else
        ImGui::TextUnformatted(text.c_str());
}

} // namespace

EigenmodeWindow::EigenmodeWindow(const std::string& title)
    : UiWindow(title)
    , m_numModesToCompute(5)
    , m_currentMode(0)
    , m_hasEigenmodes(false)
    , m_animate(false)
    , m_animationSpeed(2.0f)
    , m_animationPhase(0.0f)
    , m_modeScaleFactor(1.0)
    , m_femView(nullptr)
{
    setSize(400, 300);
}

std::shared_ptr<EigenmodeWindow> EigenmodeWindow::create(const std::string& title)
{
    return std::make_shared<EigenmodeWindow>(title);
}

void EigenmodeWindow::setFemView(::FemViewWindow* view)
{
    m_femView = view;
}

bool EigenmodeWindow::hasEigenmodes() const
{
    return m_hasEigenmodes;
}

void EigenmodeWindow::setEigenvalues(const std::vector<double>& eigenvalues)
{
    m_eigenvalues = eigenvalues;
}

void EigenmodeWindow::setHasEigenmodes(bool hasEigenmodes)
{
    m_hasEigenmodes = hasEigenmodes;
    if (!m_hasEigenmodes)
    {
        m_numModes = 0;
        m_currentMode = 0;
        m_animate = false;
        m_animationPhase = 0.0f;
    }
}

void EigenmodeWindow::setNumModes(int numModes)
{
    // The mode slider ranges over what was computed, which is not necessarily
    // what the "Modes" field says: the field can be edited afterwards, and a
    // solve that finds the structure unstable computes modes on its own.

    m_numModes = numModes > 0 ? numModes : 0;
    if (m_currentMode >= numModes)
        m_currentMode = numModes > 0 ? numModes - 1 : 0;
}

void ofui::EigenmodeWindow::setCurrentMode(int mode)
{
    m_currentMode = mode;
}

void EigenmodeWindow::setModeScaleFactor(double factor)
{
    m_modeScaleFactor = factor;
}

void EigenmodeWindow::setScaleSliderMax(float maxVal)
{
    m_sliderMax = maxVal;
}

int EigenmodeWindow::getCurrentMode() const
{
    return m_currentMode;
}

void ofui::EigenmodeWindow::setAnimate(bool animate)
{
    m_animate = animate;
}

bool EigenmodeWindow::isAnimate() const
{
    return m_animate;
}

float EigenmodeWindow::getAnimationPhase() const
{
    return m_animationPhase;
}

double EigenmodeWindow::getModeScaleFactor() const
{
    return m_modeScaleFactor;
}

void EigenmodeWindow::updateAnimationPhase(float deltaTime)
{
    if (m_animate)
    {
        m_animationPhase += deltaTime * m_animationSpeed;
        if (m_animationPhase > 6.28318530718f) // 2*PI
            m_animationPhase -= 6.28318530718f;
    }
}

void EigenmodeWindow::doPreDraw()
{
    setName(UiProfile::instance()->has(UiFeature::EigenmodeDetails) ? advancedTitle : simpleTitle);
}

void EigenmodeWindow::doDraw()
{
    // A simple profile keeps "which mode, and how does it move": the mode
    // slider, animate, speed and scale. It drops the setup above them and the
    // eigenvalue table - modes are computed for you when a solve finds the
    // structure unstable or unloaded (see FemViewSolverHandler), so there is
    // nothing here the user has to run by hand.
    //
    // Mode indices are 0-based internally and 1-based everywhere they are shown.

    const bool details = UiProfile::instance()->has(UiFeature::EigenmodeDetails);

    if (details)
    {
        ImGui::SetNextItemWidth(ImGui::GetFontSize() * 6.0f);
        ImGui::InputInt("Modes", &m_numModesToCompute);
        if (m_numModesToCompute < 1)
            m_numModesToCompute = 1;
        if (m_numModesToCompute > 20)
            m_numModesToCompute = 20;

        ImGui::SameLine();
        if (ImGui::Button("Compute"))
            onComputeButtonClicked();

        ImGui::SameLine();
        ImGui::BeginDisabled(!m_hasEigenmodes);
        if (ImGui::Button("Clear"))
            onClearButtonClicked();
        ImGui::EndDisabled();

        ImGui::Separator();
    }

    if (!m_hasEigenmodes || m_numModes < 1)
    {
        // Without the Compute button there is nothing in this panel to press,
        // so the hint has to point at the thing that does fill it.

        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s",
                           details ? "Run analysis first to compute eigenmodes"
                                   : "Run the analysis. If the structure cannot stand up,\nthe way it moves "
                                     "appears here.");
        return;
    }

    // The table already shows the selected mode's frequency, so the line under
    // the slider is only drawn where there is no table. With a single mode and
    // a table there is nothing left for the selector to do at all.

    if (details)
    {
        if (m_numModes > 1)
        {
            drawModeSelector(false);
            ImGui::Separator();
        }
        drawModeTable();
    }
    else
        drawModeSelector(true);

    ImGui::Separator();
    drawAnimationControls(details);
}

void EigenmodeWindow::drawModeSelector(bool showDescription)
{
    // Eigenvalues arrive after the mode count, so there may briefly be a mode
    // with nothing to say about it yet.

    const bool haveValue = showDescription && m_currentMode < (int)m_eigenvalues.size();

    if (m_numModes == 1)
    {
        // One mode leaves nothing to slide between; just say what it is.

        if (haveValue)
        {
            ImGui::TextUnformatted("Mode 1:");
            ImGui::SameLine();
            descriptionText(m_eigenvalues[m_currentMode]);
        }
        return;
    }

    // The slider edits a 1-based copy so its range and readout agree with the
    // table.

    char format[32];
    std::snprintf(format, sizeof(format), "%%d / %d", m_numModes);

    int shown = m_currentMode + 1;
    if (ImGui::SliderInt("Mode", &shown, 1, m_numModes, format, ImGuiSliderFlags_AlwaysClamp) &&
        shown - 1 != m_currentMode)
    {
        m_currentMode = shown - 1;
        m_scrollToCurrent = true;
        onModeChanged(m_currentMode);
    }

    if (haveValue)
        descriptionText(m_eigenvalues[m_currentMode]);
}

void EigenmodeWindow::drawModeTable()
{
    const int rows = (int)m_eigenvalues.size();
    if (rows == 0)
        return;

    // Selection is shown by the row highlight, so colour is free to mean one
    // thing only: red is unstable.

    const int visible = rows < maxVisibleRows ? rows : maxVisibleRows;
    const float height =
        ImGui::GetTextLineHeightWithSpacing() * (visible + 1) + ImGui::GetStyle().CellPadding.y * 2.0f;

    const ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_SizingStretchProp;
    if (!ImGui::BeginTable("##modes", 2, flags, ImVec2(0.0f, height)))
        return;

    ImGui::TableSetupScrollFreeze(0, 1);
    ImGui::TableSetupColumn("#", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("Frequency", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableHeadersRow();

    for (int i = 0; i < rows; i++)
    {
        const bool unstable = m_eigenvalues[i] < 0.0;
        const bool current = (i == m_currentMode);

        ImGui::PushID(i);
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);

        if (unstable)
            ImGui::PushStyleColor(ImGuiCol_Text, unstableColor);

        const std::string index = std::to_string(i + 1);
        if (ImGui::Selectable(index.c_str(), current, ImGuiSelectableFlags_SpanAllColumns) && !current)
        {
            m_currentMode = i;
            onModeChanged(m_currentMode);
        }

        // Keep the row the slider moved to in view.

        if (current && m_scrollToCurrent)
        {
            ImGui::SetScrollHereY();
            m_scrollToCurrent = false;
        }

        ImGui::TableSetColumnIndex(1);
        ImGui::TextUnformatted(modeDescription(m_eigenvalues[i]).c_str());

        if (unstable)
            ImGui::PopStyleColor();

        ImGui::PopID();
    }

    ImGui::EndTable();
}

void EigenmodeWindow::drawAnimationControls(bool details)
{
    ImGui::Checkbox("Animate", &m_animate);

    // Speed stays in place, greyed out, while the animation is off, so toggling
    // Animate does not shift everything below it.

    ImGui::SameLine();
    const float labelWidth = ImGui::CalcTextSize("Speed").x + ImGui::GetStyle().ItemInnerSpacing.x;
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - labelWidth);
    ImGui::BeginDisabled(!m_animate);
    ImGui::SliderFloat("Speed", &m_animationSpeed, 0.1f, 10.0f, "%.1f");
    ImGui::EndDisabled();

    float scaleFactor = static_cast<float>(m_modeScaleFactor);
    if (ImGui::SliderFloat("Scale", &scaleFactor, 0.0f, m_sliderMax))
    {
        m_modeScaleFactor = static_cast<double>(scaleFactor);
        if (!m_animate && m_femView != nullptr)
            m_femView->setEigenmodeVisualization(m_currentMode);
    }

    if (details && (m_femView != nullptr))
    {
        bool inSecondary = m_femView->isEigenmodeInSecondaryView();
        if (ImGui::Checkbox("Secondary view", &inSecondary))
            m_femView->setEigenmodeInSecondaryView(inSecondary);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Show the animation in the secondary view");
    }
}

void EigenmodeWindow::onComputeButtonClicked()
{
    Logger::instance()->log(LogLevel::Info, 
        "Computing " + std::to_string(m_numModesToCompute) + " eigenmodes...");
    
    if (m_femView != nullptr)
    {
        m_femView->computeEigenmodes(m_numModesToCompute);
    }
}

void EigenmodeWindow::onClearButtonClicked()
{
    Logger::instance()->log(LogLevel::Info, "Clearing eigenmodes...");
    
    setHasEigenmodes(false);
    
    if (m_femView != nullptr)
    {
        m_femView->clearEigenmodes();
    }
}

void EigenmodeWindow::onModeChanged(int mode)
{
    Logger::instance()->log(LogLevel::Info, 
        "Switched to eigenmode " + std::to_string(mode + 1));
    
    m_animationPhase = 0.0f;
    
    if (m_femView != nullptr)
    {
        m_femView->setEigenmodeVisualization(mode);
    }
}