#ifndef _CIvfFemBeam_H_
#define _CIvfFemBeam_H_

#include <FemBeam.h>
#include <FemNode.h>

#include <ivf/IvfComposite.h>
#include <ivf/IvfSimpleLineSet.h>
#include <ivf/IvfSolidLine.h>
#include <ivf/IvfMaterial.h>
#include <ivf/IvfExtrusion.h>
#include <ivf/IvfLineSet.h>
#include <ivf/IvfImage.h>
#include <ivf/IvfTexture.h>

#include <ColorMap.h>
#include <ResultInfo.h>

#include "IvfFemNode.h"
#include "IvfColorTable.h"

#include "IvfBeamModel.h"

IvfSmartPointer(CIvfFemBeam);

class CIvfFemBeam : public CIvfComposite {
private:
	CFemBeam*			m_femBeam;
	CIvfSimpleLineSet*	m_lineSet;
	CIvfSolidLine*		m_solidLine;
	CIvfFemNode*		m_nodes[2];
	CIvfMaterial*		m_beamMaterial;
	CIvfExtrusion*		m_extrusion;

	CIvfTexture*		m_beamTexture;
	CIvfImage*			m_beamImage;

	CIvfBeamModel*		m_beamModel;

	void initResults();
	double calcNavier(double N, double My, double Mz);
public:
	CIvfFemBeam ();
	virtual ~CIvfFemBeam ();

	IvfClassInfo("CIvfFemBeam",CIvfComposite);

	// Methods

	virtual void refresh();
	void initExtrusion();

	// Get/set methods

	void setBeam(CFemBeam* beam);
	CFemBeam* getBeam();
	void setNodes(CIvfFemNode* node1, CIvfFemNode* node2);

	void setBeamModel(CIvfBeamModel* beamModel);

protected:
	virtual void doCreateSelect();
	virtual void doCreateGeometry();
};
#endif
