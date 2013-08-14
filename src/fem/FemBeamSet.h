#ifndef _CFemBeamSet_H_
#define _CFemBeamSet_H_

#include "FemElementSet.h"
#include "FemBeam.h"
#include "FemBeamMaterialSet.h"

FemSmartPointer(CFemBeamSet);

class CFemBeamSet : public CFemElementSet {
protected:
	virtual CFemElement* createElement();
public:
	CFemBeamSet ();
	virtual ~CFemBeamSet ();

	FemClassInfo("CFemBeamSet",CFemElementSet);

	// Methods

	void connectMaterials(CFemBeamMaterialSet* materialSet);
};
#endif
