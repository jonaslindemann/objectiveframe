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

enum class ModelState
{
    Ok,
    NoNodes,
    NoElements,
    NoBC,
    NoLoads,
    Unstable,
    Invalid,
    Singular,
    UndefinedMaterial
};

class FrameSolver
{
private:
    ofem::BeamModel* m_beamModel;
    double m_maxNodeValue;
    ofem::Node* m_forceNode;
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

    void execute();
    void recompute();
    void update();

    void initMaxMin();
    void printMaxMin();
    void updateMaxMin(double N, double T, double Vy, double Vz, double My, double Mz, double Navier);
    double calcNavier(double N, double My, double Mz, ofem::Beam* beam);

    void setBeamModel(ofem::BeamModel* model);
    void setFeedbackForce(ofem::Node* node, double fx, double fy, double fz);
    double getMaxNodeValue();

    ModelState modelState();
};

typedef std::shared_ptr<FrameSolver> FrameSolverPtr;
