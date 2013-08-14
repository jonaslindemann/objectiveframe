#ifndef _CIvfFemNode_H_
#define _CIvfFemNode_H_

#include <FemNode.h>

#include <ivfmath/IvfVec3d.h>
#include <ivf/IvfNode.h>

#include "IvfBeamModel.h"

IvfSmartPointer(CIvfFemNode);

// public class: CIvfFemNode
class CIvfFemNode : public CIvfNode {
private:
	CFemNode* m_femNode;
	bool m_directRefresh;
	CIvfBeamModel* m_beamModel;
public:

	CIvfFemNode ();
	virtual ~CIvfFemNode ();

	IvfClassInfo("CIvfFemNode",CIvfNode);

	void refresh();

	CFemNode* getFemNode();
	void setFemNode(CFemNode* node);

	void setBeamModel(CIvfBeamModel* model);
	void setDirectRefresh(bool flag);
	void getDisplacedPosition(double &x, double &y, double &z);

	virtual void setPosition (const double x, const double y, const double z);
	virtual void setPosition (CIvfShape* shape);
	virtual void setPosition (CIvfVec3d* point);
protected:
	virtual void doCreateGeometry();
};
#endif
