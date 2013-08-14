#ifndef _CFemElementLoadSet_H_
#define _CFemElementLoadSet_H_

#include "FemLoadSet.h"

FemSmartPointer(CFemElementLoadSet);

class CFemElementLoadSet : public CFemLoadSet  
{
public:
	CFemElementLoadSet();
	virtual ~CFemElementLoadSet();

	// Class info

	FemClassInfo("CFemElementLoadSet",CFemLoadSet);

protected:
	virtual CFemLoad* createLoad();
};

#endif 
