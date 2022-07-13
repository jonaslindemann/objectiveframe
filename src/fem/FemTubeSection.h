#ifndef _CFemTubeSection_H_
#define _CFemTubeSection_H_

#include "FemSection.h"

FemSmartPointer(FemTubeSection);

class FemTubeSection : public FemSection {
public:
    FemTubeSection ();
    virtual ~FemTubeSection ();

    FemClassInfo("FemTubeSection",FemSection);
};
#endif
