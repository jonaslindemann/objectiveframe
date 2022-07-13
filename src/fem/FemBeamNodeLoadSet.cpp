#include "FemBeamNodeLoadSet.h"

// ------------------------------------------------------------
FemBeamNodeLoadSet::FemBeamNodeLoadSet()
    :FemNodeLoadSet()
{

}

// ------------------------------------------------------------
FemBeamNodeLoadSet::~FemBeamNodeLoadSet()
{

}

// ------------------------------------------------------------
FemLoad* FemBeamNodeLoadSet::createLoad()
{
    return new FemBeamNodeLoad();
}

