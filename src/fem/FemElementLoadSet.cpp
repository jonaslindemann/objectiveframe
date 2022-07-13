#include "FemElementLoadSet.h"

#include "FemElementLoad.h"

FemElementLoadSet::FemElementLoadSet()
    :FemLoadSet()
{

}

FemElementLoadSet::~FemElementLoadSet()
{

}

FemLoad* FemElementLoadSet::createLoad()
{
    return new FemElementLoad();
}
