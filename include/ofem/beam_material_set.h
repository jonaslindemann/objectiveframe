#pragma once

#include <ofem/beam_material.h>
#include <ofem/material_set.h>

namespace ofem {

class BeamMaterialSet : public MaterialSet {
public:
    BeamMaterialSet();
    virtual ~BeamMaterialSet();

protected:
    virtual Material *createMaterial();
};

} // namespace ofem
