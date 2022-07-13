#ifndef _CFemBeamNodeLoadSet_
#define _CFemBeamNodeLoadSet_

#include "FemNodeLoadSet.h"
#include "FemBeamNodeLoad.h"

FemSmartPointer(CFEmBeamNodeLoadSet);

class FemBeamNodeLoadSet : public FemNodeLoadSet {
private:

public:
    FemBeamNodeLoadSet();
    virtual ~FemBeamNodeLoadSet();

    FemClassInfo("FemBeamNodeLoadSet",FemNodeLoadSet);
protected:
    FemLoad* createLoad();
};

#endif
