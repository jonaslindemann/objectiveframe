#include "FemBeamMaterialSet.h"

// ------------------------------------------------------------
FemBeamMaterialSet::FemBeamMaterialSet()
    :FemMaterialSet()
{

}

// ------------------------------------------------------------
FemBeamMaterialSet::~FemBeamMaterialSet()
{

}

// ------------------------------------------------------------
FemMaterial* FemBeamMaterialSet::createMaterial()
{
    return new FemBeamMaterial();
}
