#include "FemBeamLoadSet.h"

#include "FemBeamLoad.h"

// ------------------------------------------------------------
FemBeamLoadSet::FemBeamLoadSet()
    :FemElementLoadSet()
{

}

// ------------------------------------------------------------
FemBeamLoadSet::~FemBeamLoadSet()
{

}

// ------------------------------------------------------------
FemLoad* FemBeamLoadSet::createLoad()
{
    return new FemBeamLoad();
}

