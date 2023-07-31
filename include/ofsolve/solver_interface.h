#pragma once

#include <ofem/beam_model.h>

namespace ofsolver {

enum class ModelState {
    Ok,
    NoNodes,
    NoElements,
    NoBC,
    NoLoads,
    Unstable,
    Invalid,
    Singular,
    UndefinedMaterial,
    SetupFailed,
    SolveFailed,
    RecomputeFailed
};

class SolverInterface {
public:
    virtual void execute() = 0;
    virtual void recompute() = 0;
    virtual void update() = 0;

    virtual void initMaxMin() = 0;
    virtual void printMaxMin() = 0;
    virtual void updateMaxMin(double N, double T, double Vy, double Vz, double My, double Mz, double Navier) = 0;
    virtual double calcNavier(double N, double My, double Mz, ofem::Beam *beam) = 0;

    virtual void setBeamModel(ofem::BeamModel *model) = 0;
    virtual void setFeedbackForce(ofem::Node *node, double fx, double fy, double fz) = 0;
    virtual double getMaxNodeValue() = 0;

    virtual ModelState modelState() = 0;
};

} // namespace ofsolver
