#ifndef _CFemBeamLoadSet_
#define _CFemBeamLoadSet_

#include "FemElementLoadSet.h"
#include "FemLoad.h"

FemSmartPointer(FemBeamLoadSet);

class FemBeamLoadSet : public FemElementLoadSet {
public:
    FemBeamLoadSet();
    virtual ~FemBeamLoadSet();

    FemClassInfo("FemBeamLoadSet",FemElementLoadSet);
protected:
    FemLoad* createLoad();
};

#endif
