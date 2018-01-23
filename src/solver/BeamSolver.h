#ifndef _CFemInternalSolver_h_
#define _CFemInternalSolver_h_


#include <FemBeamModel.h>

#include <iostream>
#include <iomanip>
#include <cmath>


#include <include.h>

#include <newmat.h>
#include <newmatio.h>
#include <newmatap.h>

#include <ResultInfo.h>

#ifdef use_namespace
using namespace NEWMAT;
#endif

#define BS_NO_ERROR			0
#define BS_NO_NODES			1
#define BS_NO_ELEMENTS		2
#define BS_NO_BC			3
#define BS_NO_LOADS			4
#define BS_UNSTABLE			5
#define BS_INVALID_MODEL	6
#define BS_SINGULAR			7
#define BS_UNDEFINED_MATERIAL 8

class CFemInternalSolver {
private:
    CFemBeamModel* m_beamModel;
    double m_maxNodeValue;
    CFemNode* m_forceNode;
    double m_force[3];
    LinearEquationSolver* m_X;
    int m_nDof;
    int m_nVars;
    ColumnVector m_f;
    ColumnVector m_fsys;
    ColumnVector m_gdof;
    ColumnVector m_ldof;
    ColumnVector m_GlobalA;
    ColumnVector m_a;
    int m_errorStatus;


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

    CResultInfo* m_resultInfo;

public:
    double calcNavier(double N, double My, double Mz, CFemBeam* beam);
    void setResultInfo(CResultInfo* resultInfo);
    CFemInternalSolver();
    virtual ~CFemInternalSolver();

    void execute();
    void recompute();
    void update();

    void initMaxMin();
    void printMaxMin();
    void updateMaxMin(double N, double T, double Vy, double Vz, double My, double Mz, double Navier);

    void setBeamModel(CFemBeamModel* model);
    void setFeedbackForce(CFemNode* node, double fx, double fy, double fz);
    double getMaxNodeValue();
    int getLastError();
};

#endif
