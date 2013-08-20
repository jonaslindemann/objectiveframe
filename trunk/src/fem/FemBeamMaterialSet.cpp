#include "FemBeamMaterialSet.h"

// ------------------------------------------------------------
CFemBeamMaterialSet::CFemBeamMaterialSet()
    :CFemMaterialSet()
{

}

// ------------------------------------------------------------
CFemBeamMaterialSet::~CFemBeamMaterialSet()
{

}

// ------------------------------------------------------------
CFemMaterial* CFemBeamMaterialSet::createMaterial()
{
    return new CFemBeamMaterial();
}
