#ifndef _CIvfFemBeamLoad_H_
#define _CIvfFemBeamLoad_H_

#include <FemBeamLoad.h>
#include <ivf/IvfShape.h>
#include <ivf/IvfQuadSet.h>
#include <ivf/IvfExtrArrow.h>
#include <ivf/IvfMaterial.h>

#include "IvfColorTable.h"
#include "IvfBeamModel.h"

#include <ivf/ivfconfig.h>

namespace std {};
using namespace std;

IvfSmartPointer(CIvfFemBeamLoad);

// public class: CIvfFemBeamLoad
class CIvfFemBeamLoad : public CIvfShape {
private:
	vector<CIvfQuadSet*> m_q;
	CFemBeamLoad* m_beamLoad;
	vector<CIvfExtrArrow*> m_arrow;
	CIvfMaterial* m_extrMaterial;
	CIvfMaterial* m_arrowMaterial;
	CIvfColorTable* m_colorTable;

	CIvfBeamModel* m_beamModel;

	void initExtrusion();
public:
	CIvfFemBeamLoad ();
	virtual ~CIvfFemBeamLoad ();

	IvfClassInfo("CIvfFemBeamLoad",CIvfShape);

	void setColorTable(CIvfColorTable* colorTable);
	void setBeamModel(CIvfBeamModel* model);


	void setBeamLoad(CFemBeamLoad* load);

	virtual void refresh();
protected:
	virtual void createSelect();
	virtual void createGeometry();
};
#endif
