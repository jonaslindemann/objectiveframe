#ifndef _CFemNodeLoadSet_H_
#define _CFemNodeLoadSet_H_

#include "FemLoadSet.h"

FemSmartPointer(CFemNodeLoadSet);

class CFemNodeLoadSet : public CFemLoadSet  
{
public:
	CFemNodeLoadSet();
	virtual ~CFemNodeLoadSet();

	// Class info

	FemClassInfo("CFemNodeLoadSet",CFemLoadSet);

protected:
	CFemLoad* createLoad();
};

#endif 
