// Implementation of: public class CFemBeamModel

#include "FemBeamModel.h"
#include "FemBeamSet.h"

// ------------------------------------------------------------
CFemBeamModel::CFemBeamModel ()
    :CFemModel()
    //TODO: check and complete member initialisation list!
{
}

// ------------------------------------------------------------
CFemBeamModel::~CFemBeamModel ()
{
}

// ------------------------------------------------------------
void CFemBeamModel::connectMaterials()
{
    CFemBeamSet* elementSet = this->getElementSet();
    elementSet->connectMaterials(this->getMaterialSet());
}

// ------------------------------------------------------------
CFemElementSet* CFemBeamModel::createElementSet()
{
    return new CFemBeamSet();
}

// ------------------------------------------------------------
CFemMaterialSet* CFemBeamModel::createMaterialSet()
{
    return new CFemBeamMaterialSet();
}

// ------------------------------------------------------------
CFemBeamSet* CFemBeamModel::getElementSet()
{
    return (CFemBeamSet*)(CFemModel::getElementSet());
}

// ------------------------------------------------------------
CFemBeamMaterialSet* CFemBeamModel::getMaterialSet()
{
    return (CFemBeamMaterialSet*)(CFemModel::getMaterialSet());
}

// ------------------------------------------------------------
CFemElementLoadSet* CFemBeamModel::createElementLoadSet()
{
    return new CFemBeamLoadSet();
}

// ------------------------------------------------------------
CFemNodeLoadSet* CFemBeamModel::createNodeLoadSet()
{
    return new CFemBeamNodeLoadSet();
}

// ------------------------------------------------------------
CFemNodeBCSet* CFemBeamModel::createBCSet()
{
    return new CFemBeamNodeBCSet();
}
