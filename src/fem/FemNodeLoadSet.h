#ifndef _CFemNodeLoadSet_H_
#define _CFemNodeLoadSet_H_

#include "FemLoadSet.h"

FemSmartPointer(FemNodeLoadSet);

class FemNodeLoadSet : public FemLoadSet
{
public:
    FemNodeLoadSet();
    virtual ~FemNodeLoadSet();

    // Class info

    FemClassInfo("FemNodeLoadSet",FemLoadSet);

protected:
    FemLoad* createLoad();
};

#endif
