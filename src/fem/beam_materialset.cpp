#include <ofem/beam_material_set.h>

using namespace ofem;

BeamMaterialSet::BeamMaterialSet() : MaterialSet()
{
}

BeamMaterialSet::~BeamMaterialSet()
{
}

Material *BeamMaterialSet::createMaterial()
{
    return new BeamMaterial();
}
