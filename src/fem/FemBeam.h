#ifndef _CFemBeam_H_
#define _CFemBeam_H_

#include "FemElement.h"
#include "FemDof.h"
#include "FemBeamMaterial.h"
#include "FemNode.h"

#include <vector>

FemSmartPointer(FemBeam);

class FemBeam : public FemElement {
private:
    void eulrot(double *axis, double angle, double *u, double *v);
    FemBeamMaterialPtr    m_material;
    long				  m_materialIndex;
    double                m_beamRotation;
    int					  m_evaluationPoints;
public:
    FemBeam ();
    virtual ~FemBeam ();

    FemClassInfo("FemBeam",FemElement);

    // Methods

    // Get/set methods

    void getOrientationZ(double &ex, double &ey, double &ez);
    void getOrientationY(double &ex, double &ey, double &ez);
    FemBeamMaterial* getMaterial();
    void setMaterial(FemBeamMaterial* material);
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
