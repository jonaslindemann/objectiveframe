#include "FemNodeBCSet.h"

#include "FemNodeBC.h"

// ------------------------------------------------------------
FemNodeBCSet::FemNodeBCSet()
    :FemBCSet()
{

}

// ------------------------------------------------------------
FemNodeBCSet::~FemNodeBCSet()
{

}

// ------------------------------------------------------------
FemBC* FemNodeBCSet::createBC()
{
    return (FemBC*) new FemNodeBC();
}

