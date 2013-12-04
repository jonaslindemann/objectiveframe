#include "FemNodeBCSet.h"

#include "FemNodeBC.h"

// ------------------------------------------------------------
CFemNodeBCSet::CFemNodeBCSet()
    :CFemBCSet()
{

}

// ------------------------------------------------------------
CFemNodeBCSet::~CFemNodeBCSet()
{

}

// ------------------------------------------------------------
CFemBC* CFemNodeBCSet::createBC()
{
    return (CFemBC*) new CFemNodeBC();
}

