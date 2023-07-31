#include <ofui/scale_window.h>

#ifdef USE_FEMVIEW
#include <FemView.h>
#else
#include <FemWidget.h>
#endif

#include <vfem/beam_model.h>

using namespace ofui;

ScaleWindow::ScaleWindow(const std::string name)
    : UiWindow(name), m_view{nullptr}, m_scaleFactor{1.0f}, m_lockScaleFactor{false}, m_animate{false},
      m_animationSpeed{0.02f}, m_maxScale{1.0f}, m_minScale{1.0f}
{
}

ScaleWindow::~ScaleWindow()
{
}
#ifdef USE_FEMVIEW
void ofui::ScaleWindow::setView(FemViewWindow *view)
{
    m_view = view;
}
#else
void ScaleWindow::setWidget(FemWidget *femWidget)
{
    m_view = femWidget;
    m_scaleFactor = float(m_view->getScalefactor());
    m_lockScaleFactor = m_view->isScaleFactorLocked();
}
#endif

void ScaleWindow::update()
{
    m_scaleFactor = float(m_view->getScalefactor());
    m_lockScaleFactor = m_view->isScaleFactorLocked();
    m_maxScale = float(m_view->getModel()->maxScale());
    m_minScale = float(m_view->getModel()->minScale());
}

std::shared_ptr<ScaleWindow> ScaleWindow::create(const std::string name)
{
    return std::make_shared<ScaleWindow>(name);
}

void ScaleWindow::doDraw()
{
    ImGui::Dummy(ImVec2(150.0, 0.0));

    this->update();

    ImGui::DragFloat("Scale factor", &m_scaleFactor, float(m_view->autoScaleFactor() * 0.01f),
                     float(m_view->autoScaleFactor()), float(m_view->autoScaleFactor() * 2.0f));
    ImGui::Separator();
    ImGui::Checkbox("Lock scale factor", &m_lockScaleFactor);
    ImGui::Checkbox("Animate", &m_animate);
    if (m_animate) {
        ImGui::SliderFloat("Animation speed", &m_animationSpeed, 0.01f, 0.2f);
    }
    ImGui::Separator();
    ImGui::SliderFloat("Max value scale", &m_maxScale, 0.1f, 10.0f);
    ImGui::SliderFloat("Min value scale", &m_minScale, 0.1f, 10.0f);

    if (m_view != nullptr) {
        if (m_animate)
            m_view->setScalefactor(m_view->autoScaleFactor() *
                                   (0.5 + 0.5 * sin(ImGui::GetFrameCount() * m_animationSpeed)));
        else
            m_view->setScalefactor(m_scaleFactor);

        if (m_lockScaleFactor)
            m_view->lockScaleFactor();
        else
            m_view->unlockScaleFactor();

        m_view->getModel()->setMaxScale(m_maxScale);
        m_view->getModel()->setMinScale(m_minScale);
    }
}
