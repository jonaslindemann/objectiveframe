#ifndef _CFemBeamNodeLoadSet_
#define _CFemBeamNodeLoadSet_

#include "FemNodeLoadSet.h"
#include "FemBeamNodeLoad.h"

FemSmartPointer(CFEmBeamNodeLoadSet);

class CFemBeamNodeLoadSet : public CFemNodeLoadSet {
private:

public:
	CFemBeamNodeLoadSet();
	virtual ~CFemBeamNodeLoadSet();

	FemClassInfo("CFemBeamNodeLoadSet",CFemNodeLoadSet);
protected:
	CFemLoad* createLoad();
};

#endif 
