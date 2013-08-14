#ifndef _CIvfBC_h_
#define _CIvfBC_h_

#include <ivf/IvfCone.h>
#include <ivf/IvfMaterial.h>
#include <ivf/IvfTransform.h>

#include "IvfBeamModel.h"

IvfSmartPointer(CIvfBC);

class CIvfBC : public CIvfComposite {
private:
	CIvfConePtr m_dispCone;
	CIvfConePtr m_rotCone1;
	CIvfConePtr m_rotCone2;
	CIvfTransformPtr m_dispX;
	CIvfTransformPtr m_dispY;
	CIvfTransformPtr m_dispZ;
	CIvfTransformPtr m_rotX;
	CIvfTransformPtr m_rotY;
	CIvfTransformPtr m_rotZ;

	CIvfBeamModelPtr m_beamModel;

	void initBC();
public:
	CIvfBC();
	virtual ~CIvfBC();

	IvfClassInfo("CIvfBC",CIvfComposite);
	
	void unprescribeAll();
	void prescribe(int dof, bool prescribed);

	void refresh();

	void setBeamModel(CIvfBeamModel* model);
};

#endif 
