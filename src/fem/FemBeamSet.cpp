// Implementation of: public class CFemBeamSet

#include "FemBeamSet.h"

// ------------------------------------------------------------
CFemBeamSet::CFemBeamSet ()
    :CFemElementSet()
{
}

// ------------------------------------------------------------
CFemBeamSet::~CFemBeamSet ()
{
}

// ------------------------------------------------------------
CFemElement* CFemBeamSet::createElement()
{
    return new CFemBeam();
}

// ------------------------------------------------------------
void CFemBeamSet::connectMaterials(CFemBeamMaterialSet *materialSet)
{
    for (int i=0; i<this->getSize(); i++)
    {
        CFemBeam* element = (CFemBeam*) this->getElement(i);
        if (element->getMaterialIndex()!=-1)
        {
            element->setMaterial((CFemBeamMaterial*)
                                 materialSet->getMaterial(element->getMaterialIndex()-1));
        }
    }
}

