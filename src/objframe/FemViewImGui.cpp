#include "FemView.h"

#include <imgui.h>
#include <imguifd/ImGuiFileDialog.h>
#include <imguifd/ImGuiFileDialogConfig.h>
#include <ofutil/util_functions.h>

#include <thread>

using namespace ivf;
using namespace std;
using namespace ofui;
using namespace ofsolver;

void FemViewWindow::drainScriptQueue()
{
    std::lock_guard<std::mutex> lock(m_ai.scriptQueueMutex);

    if (!m_ai.pendingScripts.empty())
    {
        auto scriptFunc = m_ai.pendingScripts.front();
        m_ai.pendingScripts.pop();
        m_scripting.calledNewModel = false;
        this->runScriptFromText(scriptFunc);

        if (m_scripting.calledNewModel)
            this->fitWorkspaceToModel();
    }
}

void FemViewWindow::drawMainMenuBar(bool &executeCalc, bool &quitApplication)
{
    if (!ImGui::BeginMainMenuBar())
        return;

    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("Start page", ""))
            m_startPopup->show();

        ImGui::Separator();

        if (ImGui::MenuItem("New", "CTRL+N"))
        {
            m_dlg.showNewFile = true;
            m_newModelPopup->nodeSize(float(m_view.relNodeSize * 100.0f));
            m_newModelPopup->loadSize(float(m_view.relLoadSize * 100.0f));
            m_newModelPopup->lineRadius(float(m_view.relLineRadius * 100.0f));
            m_newModelPopup->modelSize(float(this->getWorkspace()));
            m_newModelPopup->show();
        }

        if (ImGui::MenuItem("Open", "CTRL+O"))
            m_dlg.openFile = true;

        if (ImGui::MenuItem("Save", "Ctrl+S"))
            m_dlg.saveFile = true;

        if (ImGui::MenuItem("Save as", "Ctrl+Shift+S"))
            m_dlg.saveFileAs = true;

        ImGui::Separator();

        if (ImGui::MenuItem("Save as CALFEM...", ""))
            m_dlg.saveAsCalfem = true;

        if (ImGui::MenuItem("Open from CALFEM...", ""))
            m_dlg.openFromCalfem = true;

        ImGui::Separator();

        if (ImGui::MenuItem("New script...", ""))
        {
            m_scriptWindow->newScript();
            m_scriptWindow->show();
        }

        if (ImGui::MenuItem("Open script...", ""))
            m_dlg.openEditScript = true;

        if (ImGui::MenuItem("Run script...", ""))
            m_dlg.openScript = true;

        ImGui::Separator();

        if (ImGui::MenuItem("Preferences...", ""))
        {
            m_settingsWindow->show();
            m_settingsWindow->center();
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Quit", "Alt+F4"))
            quitApplication = true;

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Edit"))
    {
        if (ImGui::MenuItem("Undo", "Ctrl-Z"))
            this->restoreLastSnapShot();

        if (ImGui::MenuItem("Redo", "Ctrl-Y"))
            this->restoreLastSnapShot();

        ImGui::Separator();

        if (ImGui::MenuItem("Copy", "Ctrl-C"))
            this->copy();
        if (ImGui::MenuItem("Paste", "Ctlr-V"))
            this->paste();

        ImGui::Separator();

        if (ImGui::MenuItem("Select all", "Ctrl-A"))
        {
            this->setSelectFilter(SelectMode::All);
            this->selectAllNodes();
        }
        if (ImGui::MenuItem("Select all nodes", ""))
            this->selectAllNodes();
        if (ImGui::MenuItem("Select all elements", ""))
            this->selectAllElements();

        ImGui::Separator();

        if (ImGui::MenuItem("Fix selected nodes", ""))
            this->assignNodeFixedBCSelected();
        if (ImGui::MenuItem("Fix position selected nodes", ""))
            this->assignNodePosBCSelected();

        ImGui::Separator();

        if (ImGui::MenuItem("Fix ground nodes", ""))
            this->assignNodeFixedBCGround();
        if (ImGui::MenuItem("Fix position ground nodes", ""))
            this->assignNodePosBCGround();

        ImGui::Separator();

        if (ImGui::MenuItem("Subdivide element", "Ctrl-D"))
            this->subdivideSelectedBeam();

        ImGui::Separator();

        if (ImGui::MenuItem("Mesh selected", "Ctrl-M"))
            this->meshSelectedNodes();
        if (ImGui::MenuItem("Surface selected no ground", ""))
            this->surfaceSelectedNodes(false);
        if (ImGui::MenuItem("Surface selected with ground", ""))
            this->surfaceSelectedNodes(true);

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View"))
    {
        if (ImGui::MenuItem("Fit workspace to model", "", false, m_beamModel->getNodeSet()->getSize() > 0))
            this->fitWorkspaceToModel();

        ImGui::Separator();

        if (ImGui::MenuItem("Properties...", ""))
            m_propWindow->setVisible(true);

        ImGui::Separator();

        if (ImGui::MenuItem("Node loads...", ""))
        {
            m_nodeLoadsWindow->setFemNodeLoadSet((ofem::BeamNodeLoadSet *)m_beamModel->getNodeLoadSet());
            m_nodeLoadsWindow->setVisible(true);
            this->setNeedRecalc(true);
        }

        if (ImGui::MenuItem("Element loads...", ""))
        {
            m_elementLoadsWindow->setFemLoadSet((ofem::BeamLoadSet *)m_beamModel->getElementLoadSet());
            m_elementLoadsWindow->setVisible(true);
            this->setNeedRecalc(true);
        }

        if (ImGui::MenuItem("Materials...", ""))
        {
            m_materialsWindow->setFemMaterialSet((ofem::BeamMaterialSet *)m_beamModel->getMaterialSet());
            m_materialsWindow->setVisible(true);
            this->setNeedRecalc(true);
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Script editor...", ""))
        {
            m_scriptWindow->show();
            m_scriptWindow->center();
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Secondary view...", ""))
            m_viewWindow->show();

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Mode"))
    {
        if (ImGui::MenuItem("Model", ""))
            this->setRepresentation(RepresentationMode::Fem);
        if (ImGui::MenuItem("Geometry", ""))
            this->setRepresentation(RepresentationMode::Geometry);
        if (ImGui::MenuItem("Results", ""))
            this->setRepresentation(RepresentationMode::Results);
        if (ImGui::MenuItem("Feedback", ""))
            this->setCustomMode(CustomMode::Feedback);
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Create"))
    {
        if (ImGui::MenuItem("Create using AI", ""))
            m_promptWindow->show();

        ImGui::Separator();

        for (auto &p : m_scripting.plugins)
        {
            if (ImGui::MenuItem(p->name().c_str(), ""))
            {
                this->setCustomMode(CustomMode::Structure);
                m_pluginWindow->setPlugin(p.get());
                m_pluginWindow->center();
                m_pluginWindow->show();
            }
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Calc"))
    {
        if (ImGui::MenuItem("Execute", "Ctrl-R"))
            executeCalc = true;

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Results"))
    {
        if (ImGui::MenuItem("Normal", ""))
            this->setResultType(IVF_BEAM_N);
        if (ImGui::MenuItem("Torsion", ""))
            this->setResultType(IVF_BEAM_T);
        if (ImGui::MenuItem("Shear", ""))
            this->setResultType(IVF_BEAM_V);
        if (ImGui::MenuItem("Moment", ""))
            this->setResultType(IVF_BEAM_M);
        if (ImGui::MenuItem("Navier", ""))
            this->setResultType(IVF_BEAM_NAVIER);
        if (ImGui::MenuItem("No results", ""))
            this->setResultType(IVF_BEAM_NO_RESULT);

        ImGui::Separator();

        if (ImGui::MenuItem("X-ray mode", "Alt-X"))
            this->setUseBlending(!getUseBlending());

        ImGui::Separator();

        if (ImGui::MenuItem("Scaling settings...", ""))
        {
            m_scaleWindow->show();
            m_scaleWindow->setPosition(100, 20);
        }

        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Help"))
    {
        if (ImGui::MenuItem("About...", ""))
        {
            m_aboutWindow->show();
            m_aboutWindow->center();
        }
        if (ImGui::MenuItem("Homepage...", ""))
        {
#ifdef WIN32
            ShellExecuteW(0, 0, L"https://jonaslindemann.github.io/objectiveframe/", 0, 0, SW_SHOW);
#endif
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Log...", ""))
        {
            m_logWindow->show();
            m_logWindow->center();
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Diagnostics", ""))
            m_dlg.showDiagnostics = !m_dlg.showDiagnostics;

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}

void FemViewWindow::drawPopups()
{
    m_newModelPopup->draw();

    if (m_newModelPopup->closed())
    {
        if (m_newModelPopup->modalResult() == PopupResult::OK)
        {
            m_view.relNodeSize = m_newModelPopup->nodeSize() / 100.0;
            m_view.relLineRadius = m_newModelPopup->lineRadius() / 100.0;
            m_view.relLoadSize = m_newModelPopup->loadSize() / 100.0;
            this->setWorkspace(m_newModelPopup->modelSize());
            this->newModel();
        }
        else if (m_newModelPopup->modalResult() == PopupResult::CANCEL)
        {
            log("Cancel pressed");
        }
    }

    m_notificationOverlay->draw(ImGui::GetIO().DeltaTime);

    m_startPopup->draw();

    if (m_dlg.showStyleEditor)
        ImGui::ShowStyleEditor();

    if (m_dlg.showMetricsWindow)
        ImGui::ShowMetricsWindow(&m_dlg.showMetricsWindow);

    if (m_dlg.showDiagnostics)
        ImGui::ShowDemoWindow(&m_dlg.showDiagnostics);
}

void FemViewWindow::drawFileDialogs()
{
    if (!m_view.useImGuiFileDialogs)
        return;

    auto openFileDialog = [](const char *key, const char *title, const char *filter, const char *configKey) {
        IGFD::FileDialogConfig config;
        config.path = ofutil::get_config_value(configKey, ofutil::samples_folder());

        auto viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x + 100.0, viewport->WorkPos.y + 20.0), ImGuiCond_Always);
        ImGuiFileDialog::Instance()->OpenDialog(key, title, filter, config);
    };

    if (m_dlg.openFile)
        openFileDialog("Open model", "Choose File", ".df3", "last_dir");

    if (m_dlg.saveFileAs)
        openFileDialog("Save model", "Choose File", ".df3", "last_dir");

    if (m_dlg.saveFile)
    {
        if (m_paths.fileName == "noname.df3")
        {
            openFileDialog("Save model", "Choose File", ".df3", "last_dir");
        }
        else
        {
            m_beamModel->save();
            if (m_view.saveScreenShot)
                this->saveScreenShot(m_paths.fileName + ".png");
            m_dlg.saveFile = false;
        }
    }

    if (m_dlg.saveAsCalfem)
        openFileDialog("Save as CALFEM", "Choose File", ".py", "last_calfem_dir");

    if (m_dlg.openFromCalfem)
        openFileDialog("Open from CALFEM", "Choose File", ".py", "last_calfem_dir");

    if (m_dlg.openScript)
        openFileDialog("Open script", "Choose File", ".chai", "last_script_dir");

    if (m_dlg.openEditScript)
        openFileDialog("Edit script", "Choose File", ".chai", "last_script_dir");

    if (m_dlg.newScript)
        openFileDialog("New script", "Choose File", ".chai", "last_script_dir");

    if (ImGuiFileDialog::Instance()->Display("Open model", ImGuiWindowFlags_NoCollapse, ImVec2(600, 400)))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            ofutil::set_config_value("last_dir", ImGuiFileDialog::Instance()->GetCurrentPath());
            this->open(filePathName);
        }
        ImGuiFileDialog::Instance()->Close();
        m_dlg.openFile = false;
    }

    if (ImGuiFileDialog::Instance()->Display("Save model", ImGuiWindowFlags_NoCollapse, ImVec2(600, 400)))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            ofutil::set_config_value("last_dir", ImGuiFileDialog::Instance()->GetCurrentPath());

            if (!filePathName.empty())
            {
                this->setFileName(filePathName);
                m_beamModel->setFileName(m_paths.fileName);
                m_beamModel->save();
                if (m_view.saveScreenShot)
                    this->saveScreenShot(m_paths.fileName + ".png");
            }
        }
        ImGuiFileDialog::Instance()->Close();
        m_dlg.saveFileAs = false;
        m_dlg.saveFile = false;
    }

    if (ImGuiFileDialog::Instance()->Display("Save as CALFEM", ImGuiWindowFlags_NoCollapse, ImVec2(600, 400)))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            ofutil::set_config_value("last_calfem_dir", ImGuiFileDialog::Instance()->GetCurrentPath());

            if (!filePathName.empty())
                this->exportAsCalfem(filePathName);
        }
        ImGuiFileDialog::Instance()->Close();
        m_dlg.saveAsCalfem = false;
    }

    if (ImGuiFileDialog::Instance()->Display("Open from CALFEM", ImGuiWindowFlags_NoCollapse, ImVec2(600, 400)))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            ofutil::set_config_value("last_calfem_dir", ImGuiFileDialog::Instance()->GetCurrentPath());

            if (!filePathName.empty())
                this->importAsCalfem(filePathName);
        }
        ImGuiFileDialog::Instance()->Close();
        m_dlg.openFromCalfem = false;
    }

    if (ImGuiFileDialog::Instance()->Display("Open script", ImGuiWindowFlags_NoCollapse, ImVec2(600, 400)))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            ofutil::set_config_value("last_script_dir", ImGuiFileDialog::Instance()->GetCurrentPath());

            if (!filePathName.empty())
            {
                std::thread([this, filePathName]() {
                    try
                    {
                        this->lockSceneRendering();
                        this->runScript(filePathName);
                        this->unlockSceneRendering();
                        this->redraw();
                    } catch (const std::exception &e)
                    {
                        this->unlockSceneRendering();
                        log("Script execution error: " + std::string(e.what()));
                    }
                }).detach();
            }
        }
        ImGuiFileDialog::Instance()->Close();
        m_dlg.openScript = false;
    }

    if (ImGuiFileDialog::Instance()->Display("Edit script", ImGuiWindowFlags_NoCollapse, ImVec2(600, 400)))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            ofutil::set_config_value("last_script_dir", ImGuiFileDialog::Instance()->GetCurrentPath());

            if (!filePathName.empty())
            {
                m_scriptWindow->open(filePathName);
                m_scriptWindow->show();
                m_scriptWindow->center();
            }
        }
        ImGuiFileDialog::Instance()->Close();
        m_dlg.openEditScript = false;
    }

    if (ImGuiFileDialog::Instance()->Display("New script", ImGuiWindowFlags_NoCollapse, ImVec2(600, 400)))
    {
        if (ImGuiFileDialog::Instance()->IsOk())
        {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            ofutil::set_config_value("last_script_dir", ImGuiFileDialog::Instance()->GetCurrentPath());

            if (!filePathName.empty())
            {
                m_scriptWindow->open(filePathName);
                m_scriptWindow->show();
                m_scriptWindow->center();
            }
        }
        ImGuiFileDialog::Instance()->Close();
        m_dlg.newScript = false;
    }
}
