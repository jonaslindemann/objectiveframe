#include "FemNodeLoadSet.h"

#include "FemNodeLoad.h"

// ------------------------------------------------------------
FemNodeLoadSet::FemNodeLoadSet()
    :FemLoadSet()
{

}

// ------------------------------------------------------------
FemNodeLoadSet::~FemNodeLoadSet()
{

}

// ------------------------------------------------------------
FemLoad* FemNodeLoadSet::createLoad()
{
    return new FemNodeLoad();
}
