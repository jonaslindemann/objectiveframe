#ifndef _CIvfBeamModel_H_
#define _CIvfBeamModel_H_

#include <ivf/IvfScene.h>
#include <ivf/IvfNode.h>
#include <FemBeamModel.h>
#include "IvfColorTable.h"

#include <ColorMap.h>
#include <ResultInfo.h>

#define IVF_BEAM_N 0
#define IVF_BEAM_T 1
#define IVF_BEAM_V 2
#define IVF_BEAM_M 3
#define IVF_BEAM_NAVIER 4
#define IVF_BEAM_NO_RESULT -1

#define IVF_BEAM_LINESET    0
#define IVF_BEAM_SOLID      1
#define IVF_BEAM_EXTRUSION  2
#define IVF_BEAM_RESULTS    3

#define IVF_NODE_GEOMETRY      0
#define IVF_NODE_DISPLACEMENT  1

FemSmartPointer(CIvfBeamModel);

class CIvfBeamModel : public CFemBeamModel {
private:
	double m_nodeSize;
	double m_lineRadius;
	int m_lineSides;
	double m_loadSize;
	double m_beamLoadSize;
	double m_scaleFactor;

	int m_beamType;
	int m_resultType;
	CIvfNode::TNodeType m_nodeType;

	CIvfCompositePtr m_scene;
	CIvfMaterialPtr m_nodeMaterial;
	CIvfMaterialPtr m_beamMaterial;
	CIvfColorTablePtr m_colorTable;

	CColorMap* m_colorMapPos;
	CColorMap* m_colorMapNeg;
	CColorMap* m_colorMapStd;
	CResultInfo* m_resultInfo;

	std::string m_colorMapPath;

public:
	CIvfBeamModel ();
	virtual ~CIvfBeamModel ();

	FemClassInfo("CIvfBeamModel",CFemBeamModel);

	void generateModel();

	void setScene(CIvfComposite* scene);
	CIvfComposite* getScene();

	void setResultInfo(CResultInfo* resultInfo);
	void setColorMaps(CColorMap* pos, CColorMap* neg, CColorMap* std);

	void setLineRadius(double radius);
	double getLineRadius();

	void setNodeSize(double size);
	double getNodeSize();

	void setNodeMaterial(CIvfMaterial* material);
	CIvfMaterial* getNodeMaterial();

	int getNodeType();
	void setNodeType(CIvfNode::TNodeType type);

	void setBeamLoadSize(double size);
	double getBeamLoadSize();

	void setLoadSize(double size);
	double getLoadSize();
	
	void setBeamMaterial(CIvfMaterial* material);
	void setLineSides(int sides);

	CIvfColorTable* getColorTable();

	void setPath(const std::string& path);

	void setScaleFactor(double factor);
	double getScaleFactor();
	
	CColorMap* getColorMapStd();
	CColorMap* getColorMapNeg();
	CColorMap* getColorMapPos();
	CResultInfo* getResultInfo();
	
	void setResultType(int type);
	int getResultType();

	void setBeamType(int type);
	int getBeamType();
};
#endif
