#include "FemNodeLoadSet.h"

#include "FemNodeLoad.h"

// ------------------------------------------------------------
CFemNodeLoadSet::CFemNodeLoadSet()
    :CFemLoadSet()
{

}

// ------------------------------------------------------------
CFemNodeLoadSet::~CFemNodeLoadSet()
{

}

// ------------------------------------------------------------
CFemLoad* CFemNodeLoadSet::createLoad()
{
    return new CFemNodeLoad();
}
