#ifndef _CFemNodeBCSet_H_
#define _CFemNodeBCSet_H_

#include "FemBCSet.h"
#include "FemBC.h"

FemSmartPointer(FemNodeBCSet);

class FemNodeBCSet : public FemBCSet
{
public:
    FemNodeBCSet();
    virtual ~FemNodeBCSet();

    // Class info

    FemClassInfo("FemNodeBCSet",FemBCSet);
protected:
    virtual FemBC* createBC();
};

#endif
