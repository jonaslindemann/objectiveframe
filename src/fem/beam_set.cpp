#include <ofem/beam_set.h>

using namespace ofem;

BeamSet::BeamSet()
    : ElementSet()
{
}

BeamSet::~BeamSet()
{
}

void ofem::BeamSet::updateNodeKinds()
{
    for (auto i = 0; i < this->getSize(); i++)
    {
        auto beam = static_cast<ofem::Beam*>(this->getElement(i));
        auto n0 = beam->getNode(0);
        auto n1 = beam->getNode(1);

        if (beam->beamType() == btBeam)
        {
            if ((n0->getKind() == nkNotConnected)||(n0->getKind() == nk3Dof))
                n0->setKind(nk6Dof);
            if ((n1->getKind() == nkNotConnected) || (n1->getKind() == nk3Dof))
                n1->setKind(nk6Dof);
        }
        else
        {
            if (n0->getKind() == nkNotConnected)
                n0->setKind(nk3Dof);
            if (n1->getKind() == nkNotConnected)
                n1->setKind(nk3Dof);        
        }
    }
}

Element* BeamSet::createElement()
{
    return new Beam();
}

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
