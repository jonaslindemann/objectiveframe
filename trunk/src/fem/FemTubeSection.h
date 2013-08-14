#ifndef _CFemTubeSection_H_
#define _CFemTubeSection_H_

#include "FemSection.h"

FemSmartPointer(CFemTubeSection);

class CFemTubeSection : public CFemSection {
public:
	CFemTubeSection ();
	virtual ~CFemTubeSection ();

	FemClassInfo("CFemTubeSection",CFemSection);
};
#endif
