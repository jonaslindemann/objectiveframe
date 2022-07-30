#include <ofem/beam_set.h>

using namespace ofem;

// ------------------------------------------------------------
BeamSet::BeamSet()
    : ElementSet()
{
}

// ------------------------------------------------------------
BeamSet::~BeamSet()
{
}

// ------------------------------------------------------------
Element* BeamSet::createElement()
{
    return new Beam();
}

// ------------------------------------------------------------
void BeamSet::connectMaterials(BeamMaterialSet* materialSet)
{
    for (int i = 0; i < this->getSize(); i++)
    {
        Beam* element = (Beam*)this->getElement(i);
        if (element->getMaterialIndex() != -1)
        {
            element->setMaterial((BeamMaterial*)
                                     materialSet->getMaterial(element->getMaterialIndex() - 1));
        }
    }
}
