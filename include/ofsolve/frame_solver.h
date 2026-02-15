#pragma once

#include <ofem/beam_model.h>

#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>

#include <include.h>

#include <newmat.h>
#include <newmatap.h>
#include <newmatio.h>

#include <ofsolve/solver_interface.h>

#include <Eigen/Dense>

namespace ofsolver {

class FrameSolver : public SolverInterface {
private:
    ofem::BeamModel *m_beamModel;
    double m_maxNodeValue;
    ofem::Node *m_forceNode;
    double m_force[3];
    std::unique_ptr<LinearEquationSolver> m_X;
    int m_nDof;
    int m_nVars;
    ColumnVector m_f;
    ColumnVector m_fsys;
    ColumnVector m_gdof;
    ColumnVector m_ldof;
    ColumnVector m_GlobalA;
    ColumnVector m_a;
    ModelState m_modelState;

    double m_maxN;
    double m_minN;
    double m_maxT;
    double m_minT;
    double m_maxM;
    double m_minM;
    double m_maxV;
    double m_minV;
    double m_maxNavier;
    double m_minNavier;

public:
    FrameSolver();
    virtual ~FrameSolver();

    static std::shared_ptr<FrameSolver> create()
    {
        return std::make_shared<FrameSolver>();
    }

    virtual void execute() override;
    virtual void recompute() override;
    virtual void update() override;

    virtual void initMaxMin() override;
    virtual void printMaxMin() override;
    virtual void updateMaxMin(double N, double T, double Vy, double Vz, double My, double Mz, double Navier) override;
    virtual double calcNavier(double N, double My, double Mz, ofem::Beam *beam) override;

    virtual void setBeamModel(ofem::BeamModel *model) override;
    virtual void setFeedbackForce(ofem::Node *node, double fx, double fy, double fz) override;
    virtual double getMaxNodeValue() override;

    virtual double getMaxReactionForce() override;
    virtual double getMaxReactionMoment() override;

    virtual ModelState modelState() override;
    
    // Eigenvalue analysis methods (not implemented for FrameSolver - use BeamSolver instead)
    virtual bool computeEigenModes(int numModes = 5) override;
    virtual bool hasEigenModes() const override;
    virtual int getNumEigenModes() const override;
    virtual double getEigenValue(int mode) const override;
    virtual void getEigenVector(int mode, Eigen::VectorXd& eigenVector) const override;
    virtual void clearEigenModes() override;
};

typedef std::shared_ptr<FrameSolver> FrameSolverPtr;

} // namespace ofsolver
