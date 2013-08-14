#ifndef _CFemNodeBCSet_H_
#define _CFemNodeBCSet_H_

#include "FemBCSet.h"
#include "FemBC.h"

FemSmartPointer(CFemNodeBCSet);

class CFemNodeBCSet : public CFemBCSet  
{
public:
	CFemNodeBCSet();
	virtual ~CFemNodeBCSet();

	// Class info

	FemClassInfo("CFemNodeBCSet",CFemBCSet);
protected:
	virtual CFemBC* createBC();
};

#endif
