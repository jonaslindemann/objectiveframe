#pragma once

#include <ofem/beam.h>
#include <ofem/beam_material_set.h>
#include <ofem/element_set.h>

namespace ofem {

SmartPointer(BeamSet);

class BeamSet : public ElementSet {
protected:
    virtual Element *createElement() override;

public:
    BeamSet();
    virtual ~BeamSet();

    ClassInfo("BeamSet", ElementSet);
    StdFactory(BeamSet);

    void updateNodeKinds();

    // Methods

    void connectMaterials(BeamMaterialSet *materialSet);
};
} // namespace ofem
