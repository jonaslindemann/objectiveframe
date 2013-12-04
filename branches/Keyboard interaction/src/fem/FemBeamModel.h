#ifndef _CFemBeamModel_H_
#define _CFemBeamModel_H_

#include "FemModel.h"
#include "FemBeamMaterialSet.h"
#include "FemBeamLoadSet.h"
#include "FemBeamSet.h"
#include "FemBeamNodeLoadSet.h"
#include "FemBeamNodeBCSet.h"

FemSmartPointer(CFemBeamModel);

class CFemBeamModel : public CFemModel {
protected:
    virtual CFemNodeBCSet* createBCSet();
    virtual CFemElementLoadSet* createElementLoadSet();
    virtual CFemNodeLoadSet* createNodeLoadSet();
    virtual CFemMaterialSet* createMaterialSet();
    virtual CFemElementSet* createElementSet();
    void connectMaterials();
public:
    CFemBeamModel ();
    virtual ~CFemBeamModel ();

    FemClassInfo("CFemBeamModel",CFemModel);

    // Get/set methods

    CFemBeamMaterialSet* getMaterialSet();
    CFemBeamSet* getElementSet();

};
#endif
