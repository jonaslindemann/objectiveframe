#pragma once

#include <ofem/beam_model.h>

#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>

#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/Eigenvalues>
#include <ofsolve/calfem.h>

#include <ofsolve/solver_interface.h>

namespace ofsolver {

class BeamSolver : public SolverInterface {
private:
    ofem::BeamModel *m_beamModel;
    double m_maxNodeValue;
    ofem::Node *m_forceNode;
    double m_force[3];
    // std::unique_ptr<LinearEquationSolver> m_X;
    int m_nDof;
    int m_nVars;

    calfem::SpMatrix m_Ks;
    calfem::ColVec m_f;
    calfem::ColVec m_fsys;
    calfem::ColVec m_gdof;
    calfem::ColVec m_ldof;
    calfem::ColVec m_globalA;
    calfem::ColVec m_globalQ;
    calfem::IntColVec m_bcDofs;
    calfem::ColVec m_bcVals;

    calfem::ColVec m_a;
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
    double m_maxReactionForce;
    double m_maxReactionMoment;

    calfem::SparseSolver m_sparseSolver;

    // Eigenvalue analysis members
    bool m_hasEigenModes;
    int m_numEigenModes;
    std::vector<double> m_eigenValues;
    std::vector<Eigen::VectorXd> m_eigenVectors;
    
    // Helper for eigenvalue analysis
    Eigen::MatrixXd extractFreeStiffness();

public:
    BeamSolver();
    virtual ~BeamSolver();

    static std::shared_ptr<BeamSolver> create()
    {
        return std::make_shared<BeamSolver>();
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
    
    // Eigenvalue analysis methods
    virtual bool computeEigenModes(int numModes = 5) override;
    virtual bool hasEigenModes() const override;
    virtual int getNumEigenModes() const override;
    virtual double getEigenValue(int mode) const override;
    virtual void getEigenVector(int mode, Eigen::VectorXd& eigenVector) const override;
    virtual void clearEigenModes() override;
};

typedef std::shared_ptr<BeamSolver> BeamSolverPtr;

} // namespace ofsolver
