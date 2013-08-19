#ifndef _CIvfFemNodeBC_H_
#define _CIvfFemNodeBC_H_

#include <ivf/ivfconfig.h>

namespace std {};
using namespace std;

#include "FemBeamNodeBC.h"
#include <ivf/IvfShape.h>
#include <ivf/IvfCone.h>
#include <ivf/IvfMaterial.h>
#include <ivf/IvfTransform.h>

#include "IvfColorTable.h"
#include "IvfBC.h"
#include "IvfBeamModel.h"

IvfSmartPointer(CIVfFemNodeBC);

// public class: CIvfFemNodeBC
class CIvfFemNodeBC : public CIvfShape {
private:
	CFemBeamNodeBC* m_nodeBC;
	vector<CIvfBC*> m_bc;
	CIvfMaterial* m_bcMaterial;

	CIvfBeamModel* m_beamModel;
public:
	CIvfFemNodeBC();
	virtual ~CIvfFemNodeBC();

	IvfClassInfo("CIvfFemNodeBC",CIvfShape);

	void refresh();

	void setNodeBC(CFemBeamNodeBC* bc);
	void setBeamModel(CIvfBeamModel* model);
protected:
	virtual void doCreateGeometry();
};
#endif
