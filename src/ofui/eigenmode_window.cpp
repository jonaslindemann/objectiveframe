#include <ofui/eigenmode_window.h>

#include <imgui.h>
#include <logger.h>

#include <FemView.h>

#include <cmath>
#include <string>

using namespace ofui;

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
        m_currentMode = 0;
        m_animate = false;
        m_animationPhase = 0.0f;
    }
}

void EigenmodeWindow::setNumModes(int numModes)
{
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

void EigenmodeWindow::doDraw()
{
    ImGui::Text("Eigenmode Analysis");
    ImGui::Separator();
    
    // Computation controls
    ImGui::Text("Computation:");
    ImGui::InputInt("Number of modes", &m_numModesToCompute);
    if (m_numModesToCompute < 1)
        m_numModesToCompute = 1;
    if (m_numModesToCompute > 20)
        m_numModesToCompute = 20;
    
    if (ImGui::Button("Compute Eigenmodes"))
    {
        onComputeButtonClicked();
    }
    
    ImGui::SameLine();
    
    if (ImGui::Button("Clear"))
    {
        onClearButtonClicked();
    }
    
    ImGui::Separator();
    
    // Mode visualization controls (only if we have eigenmodes)
    if (m_hasEigenmodes)
    {
        ImGui::Text("Mode Visualization:");
        
        int prevMode = m_currentMode;
        if (ImGui::SliderInt("Mode", &m_currentMode, 0, m_numModesToCompute - 1))
        {
            if (prevMode != m_currentMode)
            {
                onModeChanged(m_currentMode);
            }
        }
        
        ImGui::Checkbox("Animate", &m_animate);

        if (m_femView != nullptr)
        {
            bool inSecondary = m_femView->isEigenmodeInSecondaryView();
            if (ImGui::Checkbox("Show animation in secondary view", &inSecondary))
                m_femView->setEigenmodeInSecondaryView(inSecondary);
        }
        
        if (m_animate)
        {
            ImGui::SliderFloat("Speed", &m_animationSpeed, 0.1f, 10.0f);
        }
        
        float scaleFactor = static_cast<float>(m_modeScaleFactor);
        if (ImGui::SliderFloat("Scale Factor", &scaleFactor, 0.0f, m_sliderMax))
        {
            m_modeScaleFactor = static_cast<double>(scaleFactor);
            if (!m_animate && m_femView != nullptr)
                m_femView->setEigenmodeVisualization(m_currentMode);
        }
        
        ImGui::Separator();

        if (!m_eigenvalues.empty())
        {
            ImGui::Text("Mode stability:");
            ImGui::Separator();
            for (int i = 0; i < (int)m_eigenvalues.size(); i++)
            {
                double lambda = m_eigenvalues[i];
                bool unstable = lambda < 0.0;
                bool current = (i == m_currentMode);

                ImGui::PushID(i);

                if (current)
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.4f, 1.0f));
                else if (unstable)
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.35f, 0.35f, 1.0f));
                else
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 1.0f, 0.4f, 1.0f));

                std::string label;
                if (unstable)
                    label = "Mode " + std::to_string(i + 1) + ":  UNSTABLE";
                else
                {
                    double freq = std::sqrt(lambda) / (2.0 * 3.14159265358979323846);
                    char buf[64];
                    std::snprintf(buf, sizeof(buf), "Mode %d:  %.3f Hz", i + 1, freq);
                    label = buf;
                }

                if (ImGui::Selectable(label.c_str(), current))
                {
                    m_currentMode = i;
                    onModeChanged(m_currentMode);
                }

                ImGui::PopStyleColor();
                ImGui::PopID();
            }
        }
    }
    else
    {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
            "Run analysis first to compute eigenmodes");
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
        "Switched to eigenmode " + std::to_string(mode));
    
    m_animationPhase = 0.0f;
    
    if (m_femView != nullptr)
    {
        m_femView->setEigenmodeVisualization(mode);
    }
}