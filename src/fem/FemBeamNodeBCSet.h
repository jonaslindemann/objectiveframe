#ifndef _CFemBeamNodeBCSet_h_
#define _CFemBeamNodeBCSet_h_

#include "FemBeamNodeBC.h"
#include "FemNodeBCSet.h"

FemSmartPointer(CFemBeamNodeSetBCSet);

class CFemBeamNodeBCSet : public CFemNodeBCSet {
private:

public:
	CFemBeamNodeBCSet();
	virtual ~CFemBeamNodeBCSet();

	FemClassInfo("CFemBeamNodeBCSet",CFemNodeBCSet);
protected:
	CFemBC* createBC();
};

#endif 
