// Implementation of: public class CFemBeamModel

#include "FemBeamModel.h"
#include "FemBeamSet.h"

// ------------------------------------------------------------
FemBeamModel::FemBeamModel ()
    :FemModel()
    //TODO: check and complete member initialisation list!
{
}

// ------------------------------------------------------------
FemBeamModel::~FemBeamModel ()
{
}

// ------------------------------------------------------------
void FemBeamModel::connectMaterials()
{
    FemBeamSet* elementSet = this->getElementSet();
    elementSet->connectMaterials(this->getMaterialSet());
}

// ------------------------------------------------------------
FemElementSet* FemBeamModel::createElementSet()
{
    return new FemBeamSet();
}

// ------------------------------------------------------------
FemMaterialSet* FemBeamModel::createMaterialSet()
{
    return new FemBeamMaterialSet();
}

// ------------------------------------------------------------
FemBeamSet* FemBeamModel::getElementSet()
{
    return (FemBeamSet*)(FemModel::getElementSet());
}

// ------------------------------------------------------------
FemBeamMaterialSet* FemBeamModel::getMaterialSet()
{
    return (FemBeamMaterialSet*)(FemModel::getMaterialSet());
}

// ------------------------------------------------------------
FemElementLoadSet* FemBeamModel::createElementLoadSet()
{
    return new FemBeamLoadSet();
}

// ------------------------------------------------------------
FemNodeLoadSet* FemBeamModel::createNodeLoadSet()
{
    return new FemBeamNodeLoadSet();
}

// ------------------------------------------------------------
FemNodeBCSet* FemBeamModel::createBCSet()
{
    return new FemBeamNodeBCSet();
}
