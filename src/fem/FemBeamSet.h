#ifndef _CFemBeamSet_H_
#define _CFemBeamSet_H_

#include "FemElementSet.h"
#include "FemBeam.h"
#include "FemBeamMaterialSet.h"

FemSmartPointer(FemBeamSet);

class FemBeamSet : public FemElementSet {
protected:
    virtual FemElement* createElement();
public:
    FemBeamSet ();
    virtual ~FemBeamSet ();

    FemClassInfo("FemBeamSet",FemElementSet);

    // Methods

    void connectMaterials(FemBeamMaterialSet* materialSet);
};
#endif
