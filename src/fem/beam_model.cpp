#include <ofem/beam_model.h>
#include <ofem/beam_set.h>

using namespace ofem;

// ------------------------------------------------------------
BeamModel::BeamModel ()
    :Model()
    //TODO: check and complete member initialisation list!
{
}

// ------------------------------------------------------------
BeamModel::~BeamModel ()
{
}

// ------------------------------------------------------------
void BeamModel::connectMaterials()
{
    BeamSet* elementSet = this->getElementSet();
    elementSet->connectMaterials(this->getMaterialSet());
}

// ------------------------------------------------------------
ElementSet* BeamModel::createElementSet()
{
    return new BeamSet();
}

// ------------------------------------------------------------
MaterialSet* BeamModel::createMaterialSet()
{
    return new BeamMaterialSet();
}

// ------------------------------------------------------------
BeamSet* BeamModel::getElementSet()
{
    return (BeamSet*)(Model::getElementSet());
}

// ------------------------------------------------------------
BeamMaterialSet* BeamModel::getMaterialSet()
{
    return (BeamMaterialSet*)(Model::getMaterialSet());
}

// ------------------------------------------------------------
ElementLoadSet* BeamModel::createElementLoadSet()
{
    return new BeamLoadSet();
}

// ------------------------------------------------------------
NodeLoadSet* BeamModel::createNodeLoadSet()
{
    return new BeamNodeLoadSet();
}

// ------------------------------------------------------------
NodeBCSet* BeamModel::createBCSet()
{
    return new BeamNodeBCSet();
}
