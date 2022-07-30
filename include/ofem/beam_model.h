#pragma once

#include <ofem/beam_load_set.h>
#include <ofem/beam_material_set.h>
#include <ofem/beam_node_bc_set.h>
#include <ofem/beam_node_load_set.h>
#include <ofem/beam_set.h>
#include <ofem/model.h>

namespace ofem
{

SmartPointer(BeamModel);

class BeamModel : public Model
{
protected:
    virtual NodeBCSet* createBCSet() override;
    virtual ElementLoadSet* createElementLoadSet() override;
    virtual NodeLoadSet* createNodeLoadSet() override;
    virtual MaterialSet* createMaterialSet() override;
    virtual ElementSet* createElementSet() override;
    virtual void connectMaterials() override;

public:
    BeamModel();
    virtual ~BeamModel();

    ClassInfo("BeamModel", Model);

    // Get/set methods

    BeamMaterialSet* getMaterialSet();
    BeamSet* getElementSet();
};
}
