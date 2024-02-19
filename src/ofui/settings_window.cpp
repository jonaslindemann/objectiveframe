#include <ofui/settings_window.h>

#ifdef USE_FEMVIEW
#include <FemView.h>
#else
#include <FemWidget.h>
#endif

#include <vfem/beam_model.h>
#include <vfem/preferences.h>

using namespace ofui;

SettingsWindow::SettingsWindow(const std::string name)
    : UiWindow(name), m_size{20.0f}, m_prevSize{20.0f}, m_nodeSize{0.4f}, m_lineRadius{0.15f}, m_loadSize{7.0f},
      m_view{nullptr}, m_scaleFactor{1.0f}, m_lockScaleFactor{false}, m_showNodeNumbers{true}, m_uiScale{1.0f},
      m_lineSides{6}, m_sphereNodes{true}, m_useImGuiFileDialogs{true}, m_saveScreenShot{false}
{}

SettingsWindow::~SettingsWindow()
{}

#ifdef USE_FEMVIEW
void ofui::SettingsWindow::setFemView(FemViewWindow *view)
{
    m_view = view;
    m_size = float(m_view->getWorkspace());
    m_nodeSize = float(m_view->getRelNodeSize() * 100.0f);
    m_loadSize = float(m_view->getRelLoadSize() * 100.0f);
    m_lineRadius = float(m_view->getRelLineRadius() * 100.0f);
    // m_scaleFactor = float(m_view->getScalefactor());
    m_showNodeNumbers = vfem::Preferences::instance().showNodeNumbers();
    m_uiScale = m_view->uiScale();
    m_lineSides = static_cast<vfem::BeamModel *>(m_view->getModel())->getLineSides();

    if (vfem::Preferences::instance().useSphereNodes())
        m_sphereNodes = true;
    else
        m_sphereNodes = false;

    m_useImGuiFileDialogs = m_view->getUseImGuiFileDialogs();
    m_saveScreenShot = m_view->getSaveScreenShot();
}
#else
void SettingsWindow::setFemWidget(FemWidget *femWidget)
{
    m_view = femWidget;
    m_size = float(m_view->getWorkspace());
    m_nodeSize = float(m_view->getRelNodeSize() * 100.0f);
    m_loadSize = float(m_view->getRelLoadSize() * 100.0f);
    m_lineRadius = float(m_view->getRelLineRadius() * 100.0f);
    m_scaleFactor = float(m_view->getScalefactor());
    m_showNodeNumbers = static_cast<vfem::BeamModel *>(m_view->getModel())->showNodeNumbers();
    m_offscreenRendering = m_view->offscreenRendering();
}
#endif

void SettingsWindow::update()
{
    m_size = float(m_view->getWorkspace());
    m_nodeSize = float(m_view->getRelNodeSize() * 100.0f);
    m_loadSize = float(m_view->getRelLoadSize() * 100.0f);
    m_lineRadius = float(m_view->getRelLineRadius() * 100.0f);
    // m_scaleFactor = float(m_view->getScalefactor());
    m_uiScale = m_view->uiScale();

    static_cast<vfem::BeamModel *>(m_view->getModel())->setShowNodeNumbers(m_showNodeNumbers);
    static_cast<vfem::BeamModel *>(m_view->getModel())->setLineSides(m_lineSides);

    if (m_sphereNodes)
    {
        static_cast<vfem::BeamModel *>(m_view->getModel())->setNodeRepr(ivf::Node::NT_SPHERE);
        m_view->setSphereCursor(true);
    }
    else
    {
        static_cast<vfem::BeamModel *>(m_view->getModel())->setNodeRepr(ivf::Node::NT_CUBE);
        m_view->setSphereCursor(false);
    }

    m_view->setUseImGuiFileDialogs(m_useImGuiFileDialogs);
    m_view->setSaveScreenShot(m_saveScreenShot);
}

std::shared_ptr<SettingsWindow> SettingsWindow::create(const std::string name)
{
    return std::make_shared<SettingsWindow>(name);
}

void SettingsWindow::doDraw()
{
    // ImGui::Dummy(ImVec2(150.0, 0.0));

    this->update();

    m_size = std::nearbyint(m_size * 0.5f) * 2.0f;

    ImGui::SliderFloat("Size (units)", &m_size, 1.0f, 100.0f, "%.0f");
    ImGui::SliderFloat("Node size (%)", &m_nodeSize, 0.1f, 2.0f);
    ImGui::SliderFloat("Line radius (%)", &m_lineRadius, 0.1f, 2.0f);
    ImGui::SliderFloat("Load size (%)", &m_loadSize, 1.0f, 10.0f);
    ImGui::SliderInt("Line sides", &m_lineSides, 4, 12);

    ImGui::Separator();

    ImGui::Checkbox("Sphere nodes", &m_sphereNodes);
    ImGui::Checkbox("Show node numbers", &m_showNodeNumbers);

    ImGui::Separator();

    ImGui::SliderFloat("UI Scale", &m_uiScale, 0.5f, 3.0f);
    ImGui::Checkbox("Use ImGui file dialogs", &m_useImGuiFileDialogs);
    ImGui::Checkbox("Save screenshot on save", &m_saveScreenShot);

    ImGui::Separator();

    ImGui::TextWrapped(
        "Objective frame can expose its functionality using a rest based API, which can be started with the "
        "button below. The service currently provides no security and only listens to access from localhost.");

    ImGui::Dummy(ImVec2(0.0, 10.0));

    if (m_view->isServiceRunning())
    {
        if (ImGui::Button("Stop service"))
        {
            m_view->stopService();
        }
        ImGui::SameLine();
        ImGui::Text("Service is running at localhost:8081");
    }
    else
    {
        if (ImGui::Button("Start service"))
        {
            m_view->startService();
        }
        ImGui::SameLine();
        ImGui::Text("Service is not running.");
    }

    m_size = std::nearbyint(m_size * 0.5f) * 2.0f;

    if (m_view != nullptr)
    {
        m_view->setRelNodeSize(m_nodeSize / 100.0f);
        m_view->setRelLineRadius(m_lineRadius / 100.0f);
        m_view->setRelLoadSize(m_loadSize / 100.0f);

        if (m_size != m_prevSize)
        {
            m_view->setWorkspace(m_size, false);
            m_prevSize = m_size;
        }

        m_view->setScalefactor(m_scaleFactor);

        m_view->setUiScale(m_uiScale);

        static_cast<vfem::BeamModel *>(m_view->getModel())->setShowNodeNumbers(m_showNodeNumbers);
    }
}
