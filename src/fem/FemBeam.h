#ifndef _CFemBeam_H_
#define _CFemBeam_H_

#include "FemElement.h"
#include "FemDof.h"
#include "FemBeamMaterial.h"
#include "FemNode.h"

#include <vector>

namespace std {};
using namespace std;

FemSmartPointer(CFemBeam);

class CFemBeam : public CFemElement {
private:
    void eulrot(double *axis, double angle, double *u, double *v);
    //vector<CFemDof*>      m_extraDof[2];
    CFemBeamMaterial*     m_material;
    long				  m_materialIndex;
    double                m_beamRotation;
    int					  m_evaluationPoints;
public:
    CFemBeam ();
    virtual ~CFemBeam ();

    FemClassInfo("CFemBeam",CFemElement);

    // Methods

    // Get/set methods

    void getOrientation(double* v);
    void setOrientation(double* v);
    void getOrientationZ(double &ex, double &ey, double &ez);
    void getOrientationY(double &ex, double &ey, double &ez);
    void setOrientation(double ex, double ey, double ez);
    CFemBeamMaterial* getMaterial();
    void setMaterial(CFemBeamMaterial* material);
    long getMaterialIndex();
    double getBeamRotation();
    void setBeamRotation(double angle);
    void setEvaluationPoints(int n);
    int getEvaluationPoints();

    // IO Methods

    virtual void readFromStream(std::istream &in);
    virtual void saveToStream(std::ostream &out);
    virtual void print(std::ostream &out);
};
#endif
