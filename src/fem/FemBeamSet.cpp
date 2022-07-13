// Implementation of: public class CFemBeamSet

#include "FemBeamSet.h"

// ------------------------------------------------------------
FemBeamSet::FemBeamSet ()
    :FemElementSet()
{
}

// ------------------------------------------------------------
FemBeamSet::~FemBeamSet ()
{
}

// ------------------------------------------------------------
FemElement* FemBeamSet::createElement()
{
    return new FemBeam();
}

// ------------------------------------------------------------
void FemBeamSet::connectMaterials(FemBeamMaterialSet *materialSet)
{
    for (int i=0; i<this->getSize(); i++)
    {
        FemBeam* element = (FemBeam*) this->getElement(i);
        if (element->getMaterialIndex()!=-1)
        {
            element->setMaterial((FemBeamMaterial*)
                                 materialSet->getMaterial(element->getMaterialIndex()-1));
        }
    }
}

