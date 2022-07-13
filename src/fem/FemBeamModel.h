#ifndef _CFemBeamModel_H_
#define _CFemBeamModel_H_

#include "FemModel.h"
#include "FemBeamMaterialSet.h"
#include "FemBeamLoadSet.h"
#include "FemBeamSet.h"
#include "FemBeamNodeLoadSet.h"
#include "FemBeamNodeBCSet.h"

FemSmartPointer(FemBeamModel);

class FemBeamModel : public FemModel {
protected:
    virtual FemNodeBCSet* createBCSet();
    virtual FemElementLoadSet* createElementLoadSet();
    virtual FemNodeLoadSet* createNodeLoadSet();
    virtual FemMaterialSet* createMaterialSet();
    virtual FemElementSet* createElementSet();
    void connectMaterials();
public:
    FemBeamModel ();
    virtual ~FemBeamModel ();

    FemClassInfo("FemBeamModel",FemModel);

    // Get/set methods

    FemBeamMaterialSet* getMaterialSet();
    FemBeamSet* getElementSet();

};
#endif
