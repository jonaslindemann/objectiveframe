#ifndef _CFemBeamMaterialSet_
#define _CFemBeamMaterialSet_

#include "FemMaterialSet.h"
#include "FemBeamMaterial.h"

class FemBeamMaterialSet : public FemMaterialSet
{
public:
    FemBeamMaterialSet();
    virtual ~FemBeamMaterialSet();


protected:
    virtual FemMaterial* createMaterial();
};

#endif
