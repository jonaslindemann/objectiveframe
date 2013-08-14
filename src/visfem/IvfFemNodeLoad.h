#ifndef _CIvfFemNodeLoad_H_
#define _CIvfFemNodeLoad_H_

#include <FemBeamNodeLoad.h>
#include <ivf/IvfShape.h>
#include <ivf/IvfExtrArrow.h>

#include "IvfColorTable.h"
#include "IvfBeamModel.h"

#include <ivf/ivfconfig.h>

IvfSmartPointer(CIvfFemNodeLoad);

// public class: CIvfFemNodeLoad
class CIvfFemNodeLoad : public CIvfShape {
private:
	CFemBeamNodeLoad* m_nodeLoad;
	vector<CIvfExtrArrow*> m_arrow;
	CIvfMaterial* m_arrowMaterial;
	CIvfBeamModel* m_beamModel;

	void initArrow();
public:
	CIvfFemNodeLoad ();
	virtual ~CIvfFemNodeLoad ();

	// Class info

	IvfClassInfo("CIvfFemNodeLoad",CIvfShape);

	// Get/set methods

	void setBeamModel(CIvfBeamModel* model);
	void setNodeLoad(CFemBeamNodeLoad* nodeLoad);

	// Methods
	
	void refresh();

protected:
	virtual void doCreateSelect();
	virtual void doCreateGeometry();
};
#endif
