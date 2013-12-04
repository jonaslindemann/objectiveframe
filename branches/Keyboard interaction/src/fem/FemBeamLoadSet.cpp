#include "FemBeamLoadSet.h"

#include "FemBeamLoad.h"

// ------------------------------------------------------------
CFemBeamLoadSet::CFemBeamLoadSet()
    :CFemElementLoadSet()
{

}

// ------------------------------------------------------------
CFemBeamLoadSet::~CFemBeamLoadSet()
{

}

// ------------------------------------------------------------
CFemLoad* CFemBeamLoadSet::createLoad()
{
    return new CFemBeamLoad();
}

