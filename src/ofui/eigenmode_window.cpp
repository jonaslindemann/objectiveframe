#include <ofui/eigenmode_window.h>

#include <imgui.h>
#include <logger.h>

#include <FemView.h>

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

int EigenmodeWindow::getCurrentMode() const
{
    return m_currentMode;
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
        
        if (m_animate)
        {
            ImGui::SliderFloat("Speed", &m_animationSpeed, 0.1f, 10.0f);
        }
        
        ImGui::SliderFloat("Scale Factor", (float*)&m_modeScaleFactor, 0.0f, 10.0f);
        
        ImGui::Separator();
        
        // Display eigenvalue info would go here (will be implemented in step 8)
        ImGui::Text("Eigenvalue info:");
        ImGui::Text("  (Available after computation)");
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