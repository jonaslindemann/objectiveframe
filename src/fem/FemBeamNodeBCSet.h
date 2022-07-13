#ifndef _CFemBeamNodeBCSet_h_
#define _CFemBeamNodeBCSet_h_

#include "FemBeamNodeBC.h"
#include "FemNodeBCSet.h"

FemSmartPointer(CFemBeamNodeSetBCSet);

class FemBeamNodeBCSet : public FemNodeBCSet {
private:

public:
    FemBeamNodeBCSet();
    virtual ~FemBeamNodeBCSet();

    FemClassInfo("FemBeamNodeBCSet",FemNodeBCSet);
protected:
    FemBC* createBC();
};

#endif
