#ifndef _CFemElementLoadSet_H_
#define _CFemElementLoadSet_H_

#include "FemLoadSet.h"

FemSmartPointer(FemElementLoadSet);

class FemElementLoadSet : public FemLoadSet
{
public:
    FemElementLoadSet();
    virtual ~FemElementLoadSet();

    // Class info

    FemClassInfo("FemElementLoadSet",FemLoadSet);

protected:
    virtual FemLoad* createLoad();
};

#endif
