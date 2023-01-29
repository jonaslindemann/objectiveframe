#include <ofui/settings_window.h>

#ifdef USE_FEMVIEW
#include <FemView.h>
#else
#include <FemWidget.h>
#endif

#include <vfem/beam_model.h>

using namespace ofui;

SettingsWindow::SettingsWindow(const std::string name)
    : UiWindow(name)
    , m_size { 20.0f }
    , m_nodeSize { 0.4f }
    , m_lineRadius { 0.15f }
    , m_loadSize { 7.0f }
    , m_view { nullptr }
    , m_scaleFactor { 1.0f }
    , m_lockScaleFactor { false }
    , m_showNodeNumbers { true }
    , m_uiScale { 1.0f }
    , m_lineSides { 6 }
    , m_sphereNodes { true }
{
}

SettingsWindow::~SettingsWindow()
{
}

#ifdef USE_FEMVIEW
void ofui::SettingsWindow::setFemView(FemViewWindow* view)
{
    m_view = view;
    m_size = float(m_view->getWorkspace());
    m_nodeSize = float(m_view->getRelNodeSize() * 100.0f);
    m_loadSize = float(m_view->getRelLoadSize() * 100.0f);
    m_lineRadius = float(m_view->getRelLineRadius() * 100.0f);
    m_scaleFactor = float(m_view->getScalefactor());
    m_showNodeNumbers = static_cast<vfem::BeamModel*>(m_view->getModel())->showNodeNumbers();
    m_uiScale = m_view->uiScale();
    m_lineSides = static_cast<vfem::BeamModel*>(m_view->getModel())->getLineSides();
    if (static_cast<vfem::BeamModel*>(m_view->getModel())->getNodeRepr() == ivf::Node::NT_SPHERE)
        m_sphereNodes = true;
    else
        m_sphereNodes = false;
}
#else
void SettingsWindow::setFemWidget(FemWidget* femWidget)
{
    m_view = femWidget;
    m_size = float(m_view->getWorkspace());
    m_nodeSize = float(m_view->getRelNodeSize() * 100.0f);
    m_loadSize = float(m_view->getRelLoadSize() * 100.0f);
    m_lineRadius = float(m_view->getRelLineRadius() * 100.0f);
    m_scaleFactor = float(m_view->getScalefactor());
    m_showNodeNumbers = static_cast<vfem::BeamModel*>(m_view->getModel())->showNodeNumbers();
    m_offscreenRendering = m_view->offscreenRendering();
}
#endif

void SettingsWindow::update()
{
    m_size = float(m_view->getWorkspace());
    m_nodeSize = float(m_view->getRelNodeSize() * 100.0f);
    m_loadSize = float(m_view->getRelLoadSize() * 100.0f);
    m_lineRadius = float(m_view->getRelLineRadius() * 100.0f);
    m_scaleFactor = float(m_view->getScalefactor());
    m_uiScale = m_view->uiScale();
#ifndef USE_FEMVIEW
    m_offscreenRendering = m_view->offscreenRendering();
#endif
    static_cast<vfem::BeamModel*>(m_view->getModel())->setShowNodeNumbers(m_showNodeNumbers);
    static_cast<vfem::BeamModel*>(m_view->getModel())->setLineSides(m_lineSides);

    if (m_sphereNodes)
    {
        static_cast<vfem::BeamModel*>(m_view->getModel())->setNodeRepr(ivf::Node::NT_SPHERE);
        m_view->setSphereCursor(true);
    }
    else
    {
        static_cast<vfem::BeamModel*>(m_view->getModel())->setNodeRepr(ivf::Node::NT_CUBE);
        m_view->setSphereCursor(false);
    }
}

std::shared_ptr<SettingsWindow> SettingsWindow::create(const std::string name)
{
    return std::make_shared<SettingsWindow>(name);
}

void SettingsWindow::doDraw()
{
    ImGui::Dummy(ImVec2(150.0, 0.0));

    this->update();

    m_size = std::nearbyint(m_size * 0.5f) * 2.0f;

    ImGui::SliderFloat("Size (units)", &m_size, 1.0f, 100.0f, "%.0f");
    ImGui::SliderFloat("Node size (%)", &m_nodeSize, 0.1f, 2.0f);
    ImGui::SliderFloat("Line radius (%)", &m_lineRadius, 0.1f, 2.0f);
    ImGui::SliderFloat("Load size (%)", &m_loadSize, 1.0f, 10.0f);
    ImGui::SliderInt("Line sides", &m_lineSides, 4, 12);
    ImGui::Checkbox("Sphere nodes", &m_sphereNodes);

    ImGui::Separator();

    ImGui::DragFloat("Scale factor", &m_scaleFactor, 1.0f, 1000.0f);
    ImGui::Checkbox("Lock scale factor", &m_lockScaleFactor);

    ImGui::Separator();

    ImGui::Checkbox("Show node numbers", &m_showNodeNumbers);
    //ImGui::Checkbox("Offscreen rendering", &m_offscreenRendering);
    ImGui::SliderFloat("UI Scale", &m_uiScale, 0.5f, 3.0f);

    m_size = std::nearbyint(m_size * 0.5f) * 2.0f;

    if (m_view != nullptr)
    {
        m_view->setRelNodeSize(m_nodeSize / 100.0f);
        m_view->setRelLineRadius(m_lineRadius / 100.0f);
        m_view->setRelLoadSize(m_loadSize / 100.0f);

        m_view->setWorkspace(m_size, false);

        m_view->setScalefactor(m_scaleFactor);

        if (m_lockScaleFactor)
            m_view->lockScaleFactor();
        else
            m_view->unlockScaleFactor();
#ifndef USE_FEMVIEW
        if (m_offscreenRendering != m_view->offscreenRendering())
            m_view->setOffscreenRendering(m_offscreenRendering);
#endif
        m_view->setUiScale(m_uiScale);

        static_cast<vfem::BeamModel*>(m_view->getModel())->setShowNodeNumbers(m_showNodeNumbers);
    }
}
