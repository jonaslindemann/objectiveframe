#pragma once

#include <ofem/element_set.h>
#include <ofem/beam.h>
#include <ofem/beam_material_set.h>

namespace ofem {

    SmartPointer(BeamSet);

    class BeamSet : public ElementSet {
    protected:
        virtual Element* createElement();
    public:
        BeamSet();
        virtual ~BeamSet();

        ClassInfo("BeamSet", ElementSet);

        // Methods

        void connectMaterials(BeamMaterialSet* materialSet);
    };
}
