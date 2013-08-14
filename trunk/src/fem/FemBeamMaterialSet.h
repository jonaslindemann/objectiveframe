#ifndef _CFemBeamMaterialSet_
#define _CFemBeamMaterialSet_

#include "FemMaterialSet.h"
#include "FemBeamMaterial.h"

class CFemBeamMaterialSet : public CFemMaterialSet  
{
public:
	CFemBeamMaterialSet();
	virtual ~CFemBeamMaterialSet();


protected:
	virtual CFemMaterial* createMaterial();
};

#endif 
