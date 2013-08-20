#ifndef _CFemBeamLoadSet_
#define _CFemBeamLoadSet_

#include "FemElementLoadSet.h"
#include "FemLoad.h"

FemSmartPointer(CFemBeamLoadSet);

class CFemBeamLoadSet : public CFemElementLoadSet {
public:
    CFemBeamLoadSet();
    virtual ~CFemBeamLoadSet();

    FemClassInfo("CFemBeamLoadSet",CFemElementLoadSet);
protected:
    CFemLoad* createLoad();
};

#endif
