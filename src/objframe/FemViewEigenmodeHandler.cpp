#include "FemViewEigenmodeHandler.h"

#include "FemView.h"

#include <cmath>
#include <imgui.h>
#include <limits>
#include <string>

using namespace ivf;
using namespace std;
using namespace ofui;
using namespace ofsolver;

void FemViewEigenmodeHandler::compute(FemViewWindow &view, int numModes)
{
    if (view.m_solver.current == nullptr)
    {
        view.log("No solver available. Run analysis first.");
        return;
    }

    view.log("Computing " + std::to_string(numModes) + " eigenmodes...");

    if (view.m_solver.current->computeEigenModes(numModes))
    {
        view.log("Successfully computed " + std::to_string(view.m_solver.current->getNumEigenModes()) + " eigenmodes.");

        view.m_eigenmode.savedShowNodeNumbers = view.m_beamModel->showNodeNumbers();
        view.m_beamModel->setShowNodeNumbers(false);
        view.m_eigenmode.showing = true;

        view.m_eigenmodeWindow->setHasEigenmodes(true);
        view.m_eigenmodeWindow->setNumModes(view.m_solver.current->getNumEigenModes());
        view.m_eigenmodeWindow->setAnimate(true);

        {
            // Base scale on bounding box diagonal so it looks proportional regardless of model size.
            auto nodeSet = view.m_beamModel->getNodeSet();
            double minX = std::numeric_limits<double>::max(), maxX = std::numeric_limits<double>::lowest();
            double minY = std::numeric_limits<double>::max(), maxY = std::numeric_limits<double>::lowest();
            double minZ = std::numeric_limits<double>::max(), maxZ = std::numeric_limits<double>::lowest();
            for (size_t i = 0; i < nodeSet->getSize(); i++)
            {
                double x, y, z;
                nodeSet->getNode(i)->getCoord(x, y, z);
                minX = std::min(minX, x); maxX = std::max(maxX, x);
                minY = std::min(minY, y); maxY = std::max(maxY, y);
                minZ = std::min(minZ, z); maxZ = std::max(maxZ, z);
            }
            double dx = maxX - minX, dy = maxY - minY, dz = maxZ - minZ;
            double diagonal = std::sqrt(dx * dx + dy * dy + dz * dz);
            double autoScale = (diagonal > 0.0) ? diagonal * 0.05 : view.getWorkspace() * 0.05;
            view.m_eigenmodeWindow->setModeScaleFactor(autoScale);
            view.m_eigenmodeWindow->setScaleSliderMax(float(autoScale) * 10.0f);
        }

        view.m_eigenmodeWindow->setCurrentMode(0);
        view.m_eigenmodeWindow->show();
        view.m_tactileForce->setState(Shape::OS_OFF);
        view.m_interactionNode = nullptr;

        {
            const ImGuiViewport *vp = ImGui::GetMainViewport();
            view.m_eigenmodeWindow->setPosition(int(vp->WorkSize.x) - 600, 120);
        }

        view.setEigenmodeInSecondaryView(false);

        {
            std::vector<double> eigenvalues;
            for (int i = 0; i < view.m_solver.current->getNumEigenModes(); i++)
                eigenvalues.push_back(view.m_solver.current->getEigenValue(i));
            view.m_eigenmodeWindow->setEigenvalues(eigenvalues);
        }

        for (int i = 0; i < view.m_solver.current->getNumEigenModes(); i++)
        {
            double eigenvalue = view.m_solver.current->getEigenValue(i);
            std::string status = eigenvalue < 0 ? " (UNSTABLE)" : "";
            view.log("  Mode " + std::to_string(i + 1) + ": eigenvalue = " + std::to_string(eigenvalue) + status);
        }

        view.setEigenmodeVisualization(0);

        // An eigenmode is a displacement shape. Any result type still selected
        // from an earlier solve would colour it with values that do not belong to
        // this mode, so clear it.

        view.m_beamModel->setResultType(IVF_BEAM_NO_RESULT);
        view.setRepresentation(RepresentationMode::Displacements);
    }
    else
    {
        view.log("Failed to compute eigenmodes.");
        view.m_eigenmodeWindow->setHasEigenmodes(false);
    }
}

void FemViewEigenmodeHandler::clear(FemViewWindow &view)
{
    if (view.m_solver.current == nullptr)
    {
        view.log("No solver available.");
        return;
    }

    view.log("Clearing eigenmodes...");
    view.m_solver.current->clearEigenModes();
    view.m_eigenmodeWindow->setHasEigenmodes(false);
    view.m_eigenmodeWindow->setEigenvalues({});
    view.m_eigenmode.inSecondaryView = false;

    view.m_beamModel->clearNodeValues();
    view.m_eigenmode.showing = false;
    view.m_beamModel->setShowNodeNumbers(view.m_eigenmode.savedShowNodeNumbers);
    view.set_changed();
    view.redraw();
}

void FemViewEigenmodeHandler::setVisualization(FemViewWindow &view, int mode)
{
    if (view.m_solver.current == nullptr || !view.m_solver.current->hasEigenModes())
    {
        view.log("No eigenmodes available.");
        return;
    }

    if (mode < 0 || mode >= view.m_solver.current->getNumEigenModes())
    {
        view.log("Invalid eigenmode index: " + std::to_string(mode));
        return;
    }

    view.log("Visualizing eigenmode " + std::to_string(mode + 1));

    Eigen::VectorXd eigenvector;
    view.m_solver.current->getEigenVector(mode, eigenvector);

    auto nodeSet = view.m_beamModel->getNodeSet();
    double maxDisplacement = 0.0;

    for (size_t i = 0; i < nodeSet->getSize(); i++)
    {
        auto node = nodeSet->getNode(i);
        if (node->getKind() == ofem::nkNotConnected)
            continue;
        int maxDof = (node->getKind() == ofem::nk6Dof) ? 6 : 3;
        for (int j = 0; j < maxDof; j++)
        {
            if (node->getDof(j) != nullptr)
            {
                int dofNum = node->getDof(j)->getNumber() - 1;
                if (dofNum >= 0 && dofNum < eigenvector.size())
                    maxDisplacement = std::max(maxDisplacement, std::abs(eigenvector(dofNum)));
            }
        }
    }

    for (size_t i = 0; i < nodeSet->getSize(); i++)
    {
        auto node = nodeSet->getNode(i);
        if (node->getKind() == ofem::nk6Dof)
        {
            node->setValueSize(12);
            for (int j = 0; j < 6; j++)
            {
                double value = 0.0;
                if (node->getDof(j) != nullptr)
                {
                    int dofNum = node->getDof(j)->getNumber() - 1;
                    if (dofNum >= 0 && dofNum < eigenvector.size())
                    {
                        value = eigenvector(dofNum);
                        if (maxDisplacement > 0)
                            value /= maxDisplacement;
                    }
                }
                node->setValue(j, value);
                node->setValue(j + 6, 0.0); // clear reaction forces
            }
        }
        else if (node->getKind() == ofem::nk3Dof)
        {
            node->setValueSize(6);
            for (int j = 0; j < 3; j++)
            {
                double value = 0.0;
                if (node->getDof(j) != nullptr)
                {
                    int dofNum = node->getDof(j)->getNumber() - 1;
                    if (dofNum >= 0 && dofNum < eigenvector.size())
                    {
                        value = eigenvector(dofNum);
                        if (maxDisplacement > 0)
                            value /= maxDisplacement;
                    }
                }
                node->setValue(j, value);
                node->setValue(j + 3, 0.0); // clear reaction forces
            }
        }
    }

    if (!view.m_solver.lockScaleFactor && maxDisplacement > 0)
        view.m_beamModel->setScaleFactor(view.m_eigenmodeWindow->getModeScaleFactor());

    view.set_changed();
    view.redraw();
}

void FemViewEigenmodeHandler::updateVisualization(FemViewWindow &view, float phase)
{
    if (view.m_solver.current == nullptr || !view.m_solver.current->hasEigenModes())
        return;

    int currentMode = view.m_eigenmodeWindow->getCurrentMode();
    double animationScale = std::sin(phase);
    double modeScaleFactor = view.m_eigenmodeWindow->getModeScaleFactor();

    if (!view.m_solver.lockScaleFactor)
        view.m_beamModel->setScaleFactor(modeScaleFactor);

    Eigen::VectorXd eigenvector;
    view.m_solver.current->getEigenVector(currentMode, eigenvector);

    auto nodeSet = view.m_beamModel->getNodeSet();
    double maxDisplacement = 0.0;

    for (size_t i = 0; i < nodeSet->getSize(); i++)
    {
        auto node = nodeSet->getNode(i);
        if (node->getKind() == ofem::nkNotConnected)
            continue;
        int maxDof = (node->getKind() == ofem::nk6Dof) ? 6 : 3;
        for (int j = 0; j < maxDof; j++)
        {
            if (node->getDof(j) != nullptr)
            {
                int dofNum = node->getDof(j)->getNumber() - 1;
                if (dofNum >= 0 && dofNum < eigenvector.size())
                    maxDisplacement = std::max(maxDisplacement, std::abs(eigenvector(dofNum)));
            }
        }
    }

    for (size_t i = 0; i < nodeSet->getSize(); i++)
    {
        auto node = nodeSet->getNode(i);
        if (node->getKind() == ofem::nk6Dof)
        {
            node->setValueSize(12);
            for (int j = 0; j < 6; j++)
            {
                double value = 0.0;
                if (node->getDof(j) != nullptr)
                {
                    int dofNum = node->getDof(j)->getNumber() - 1;
                    if (dofNum >= 0 && dofNum < eigenvector.size())
                    {
                        value = eigenvector(dofNum);
                        if (maxDisplacement > 0)
                            value /= maxDisplacement;
                        value *= animationScale;
                    }
                }
                node->setValue(j, value);
            }
        }
        else if (node->getKind() == ofem::nk3Dof)
        {
            node->setValueSize(6);
            for (int j = 0; j < 3; j++)
            {
                double value = 0.0;
                if (node->getDof(j) != nullptr)
                {
                    int dofNum = node->getDof(j)->getNumber() - 1;
                    if (dofNum >= 0 && dofNum < eigenvector.size())
                    {
                        value = eigenvector(dofNum);
                        if (maxDisplacement > 0)
                            value /= maxDisplacement;
                        value *= animationScale;
                    }
                }
                node->setValue(j, value);
            }
        }
    }

    view.set_changed();
    view.redraw();
}

void FemViewEigenmodeHandler::setInSecondaryView(FemViewWindow &view, bool flag)
{
    view.m_eigenmode.inSecondaryView = flag;
    if (flag)
    {
        view.m_viewWindow->show();
        view.setRepresentation(RepresentationMode::Fem);
        view.m_beamModel->setResultType(IVF_BEAM_NO_RESULT);
        view.m_beamModel->clearNodeValues();
    }
    else
    {
        if (view.m_solver.current != nullptr && view.m_solver.current->hasEigenModes())
        {
            view.m_beamModel->setResultType(IVF_BEAM_NO_RESULT);
            view.setRepresentation(RepresentationMode::Displacements);
        }
    }
}

void FemViewEigenmodeHandler::applyAnimation(FemViewWindow &view)
{
    if (view.m_eigenmodeWindow != nullptr)
    {
        view.m_beamModel->setNodeType(IVF_NODE_DISPLACEMENT);
        view.updateEigenmodeVisualization(view.m_eigenmodeWindow->getAnimationPhase());
    }
}

void FemViewEigenmodeHandler::clearAnimation(FemViewWindow &view)
{
    view.m_beamModel->clearNodeValues();
    view.m_beamModel->setNodeType(IVF_NODE_GEOMETRY);
    view.redraw();
}
