#include <ofui/scale_window.h>

#include "FemWidget.h"
#include <vfem/beam_model.h>

using namespace ofui;

ScaleWindow::ScaleWindow(const std::string name)
    : UiWindow(name)
    , m_widget { nullptr }
    , m_scaleFactor { 1.0f }
    , m_lockScaleFactor { false }
    , m_animate { false }
    , m_animationSpeed { 0.02 }
{
}

ScaleWindow::~ScaleWindow()
{
}

void ScaleWindow::setWidget(FemWidget* femWidget)
{
    m_widget = femWidget;
    m_scaleFactor = float(m_widget->getScalefactor());
    m_lockScaleFactor = m_widget->isScaleFactorLocked();
}

void ScaleWindow::update()
{
    m_scaleFactor = float(m_widget->getScalefactor());

    m_lockScaleFactor = m_widget->isScaleFactorLocked();
}

std::shared_ptr<ScaleWindow> ScaleWindow::create(const std::string name)
{
    return std::make_shared<ScaleWindow>(name);
}

void ScaleWindow::doDraw()
{
    ImGui::Dummy(ImVec2(150.0, 0.0));

    this->update();

    ImGui::DragFloat("Scale factor", &m_scaleFactor, m_widget->autoScaleFactor() * 0.01, m_widget->autoScaleFactor(), m_widget->autoScaleFactor() * 2.0);
    ImGui::Checkbox("Lock scale factor", &m_lockScaleFactor);
    ImGui::Checkbox("Animate", &m_animate);
    if (m_animate)
    {
        ImGui::SliderFloat("Animation speed", &m_animationSpeed, 0.01, 0.2);
    }

    if (m_widget != nullptr)
    {
        if (m_animate)
            m_widget->setScalefactor(m_widget->autoScaleFactor() * (0.5 + 0.5*sin(ImGui::GetFrameCount()*m_animationSpeed)));
        else
            m_widget->setScalefactor(m_scaleFactor);

        if (m_lockScaleFactor)
            m_widget->lockScaleFactor();
        else
            m_widget->unlockScaleFactor();
    }
}
