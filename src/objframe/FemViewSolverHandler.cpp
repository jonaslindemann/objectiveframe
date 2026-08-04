#include "FemViewSolverHandler.h"

#include "FemView.h"

using namespace ivf;
using namespace std;
using namespace ofui;
using namespace ofsolver;

void FemViewSolverHandler::executeCalc(FemViewWindow &view)
{
    // Reset any active eigenmode state so the regular results can display normally
    if (view.m_eigenmodeWindow != nullptr)
    {
        view.m_eigenmodeWindow->setAnimate(false);
        view.m_eigenmodeWindow->setHasEigenmodes(false);
    }
    view.m_eigenmode.inSecondaryView = false;
    view.m_beamModel->setNodeType(IVF_NODE_GEOMETRY);

    view.m_solver.beam = BeamSolver::create();
    view.m_solver.current = view.m_solver.beam.get();

    view.m_solver.current->setBeamModel(view.m_beamModel.get());
    view.m_solver.current->execute();

    view.m_solver.saneModel = false;

    if (view.m_solver.current->modelState() != ModelState::Ok)
    {
        switch (view.m_solver.current->modelState())
        {
        case ModelState::NoNodes:
            view.notify("No nodes defined.", NotificationLevel::Warning);
            break;
        case ModelState::NoElements:
            view.notify("No elements defined.", NotificationLevel::Warning);
            break;
        case ModelState::NoBC:
            view.notify("No boundary conditions defined.", NotificationLevel::Warning);
            break;
        case ModelState::NoLoads:
            view.notify("No loads defined. Showing structural eigenmodes.", NotificationLevel::Info);
            view.computeEigenmodes(5);
            break;
        case ModelState::Unstable:
            view.notify("Structure is unstable. Showing eigenmodes.", NotificationLevel::Warning);
            view.computeEigenmodes(5);
            break;
        case ModelState::Singular:
            view.notify("System is singular. Check for free nodes.", NotificationLevel::Error);
            break;
        case ModelState::Invalid:
            view.notify("Invalid model state.", NotificationLevel::Error);
            break;
        case ModelState::UndefinedMaterial:
            view.notify("Elements without materials found.", NotificationLevel::Warning);
            break;
        case ModelState::SolveFailed:
            view.notify("Solver failed. Showing eigenmodes.", NotificationLevel::Error);
            view.computeEigenmodes(5);
            break;
        case ModelState::RecomputeFailed:
            view.notify("Recomputation failed.", NotificationLevel::Error);
            break;
        case ModelState::SetupFailed:
            view.notify("Solver setup failed.", NotificationLevel::Error);
            break;
        default:
            view.notify("Unhandled solver error.", NotificationLevel::Error);
            break;
        }
        view.m_solver.needRecalc = true;
    }
    else
    {
        view.m_solver.needRecalc = false;
        view.m_solver.saneModel = true;
    }

    auto maxNodeValue = view.m_solver.current->getMaxNodeValue();
    auto maxReactionForce = view.m_solver.current->getMaxReactionForce();
    auto maxReactionMoment = view.m_solver.current->getMaxReactionMoment();

    view.m_beamModel->setMaxReactionForce(maxReactionForce);
    view.m_beamModel->setMaxReactionMoment(maxReactionMoment);

    view.log("Max node value = " + std::to_string(maxNodeValue));

    if (!view.m_solver.lockScaleFactor)
    {
        if (maxNodeValue > 0.0)
            view.m_beamModel->setScaleFactor(view.getWorkspace() * 0.005 / maxNodeValue);
        else
            view.m_beamModel->setScaleFactor(1.0);
        view.m_solver.haveScaleFactor = true;
    }

    view.m_settingsWindow->update();
    view.m_scaleWindow->show();
    view.m_scaleWindow->setPosition(100, 20);

    view.m_loadMixerWindow->setFemNodeLoadSet((ofem::BeamNodeLoadSet *)view.m_beamModel->getNodeLoadSet());
    view.m_loadMixerWindow->show();
    view.m_loadMixerWindow->setPosition(100, 240);

    view.setRepresentation(RepresentationMode::Results);
}

void FemViewSolverHandler::recompute(FemViewWindow &view)
{
    if (view.m_solver.needRecalc)
    {
        view.m_solver.beam = BeamSolver::create();
        view.m_solver.current = view.m_solver.beam.get();

        view.m_solver.current->setBeamModel(view.m_beamModel.get());
        view.m_solver.current->execute();

        view.m_solver.saneModel = false;

        switch (view.m_solver.current->modelState())
        {
        case ModelState::NoNodes:
        case ModelState::NoElements:
        case ModelState::NoBC:
        case ModelState::NoLoads:
        case ModelState::Unstable:
        case ModelState::Singular:
        case ModelState::Invalid:
        case ModelState::UndefinedMaterial:
        case ModelState::SolveFailed:
        case ModelState::RecomputeFailed:
        case ModelState::SetupFailed:
            break;
        default:
            view.m_solver.saneModel = true;
            break;
        }

        auto maxReactionForce = view.m_solver.current->getMaxReactionForce();
        auto maxReactionMoment = view.m_solver.current->getMaxReactionMoment();

        view.m_beamModel->setMaxReactionForce(maxReactionForce);
        view.m_beamModel->setMaxReactionMoment(maxReactionMoment);

        auto maxNodeValue = view.m_solver.current->getMaxNodeValue();

        if (!view.m_solver.lockScaleFactor && !view.m_solver.haveScaleFactor)
        {
            if (maxNodeValue > 0.0)
                view.m_beamModel->setScaleFactor(view.getWorkspace() * 0.005 / maxNodeValue);
            else
                view.m_beamModel->setScaleFactor(1.0);
            view.m_solver.haveScaleFactor = true;
        }

        view.m_solver.needRecalc = false;
        view.setRepresentation(RepresentationMode::Results);
    }

    if (view.m_solver.saneModel && view.m_solver.current != nullptr)
    {
        view.m_solver.current->recompute();
        view.m_solver.current->update();

        auto maxReactionForce = view.m_solver.current->getMaxReactionForce();
        auto maxReactionMoment = view.m_solver.current->getMaxReactionMoment();

        view.m_beamModel->setMaxReactionForce(maxReactionForce);
        view.m_beamModel->setMaxReactionMoment(maxReactionMoment);

        auto maxNodeValue = view.m_solver.current->getMaxNodeValue();

        if (!view.m_solver.lockScaleFactor && !view.m_solver.haveScaleFactor)
        {
            if (maxNodeValue > 0.0)
                view.m_beamModel->setScaleFactor(view.getWorkspace() * 0.005 / maxNodeValue);
            else
                view.m_beamModel->setScaleFactor(1.0);
            view.m_solver.haveScaleFactor = true;
        }

        view.getScene()->getComposite()->refresh();
        view.redraw();
    }
}
