#ifndef _CIvfFemNode_H_
#define _CIvfFemNode_H_

#include <FemNode.h>

#include <ivfmath/Vec3d.h>
#include <ivf/Node.h>

#include "VisBeamModel.h"

IvfSmartPointer(VisFemNode);

// public class: VisFemNode
class VisFemNode : public ivf::Node {
private:
    FemNode* m_femNode;
    bool m_directRefresh;
    VisBeamModel* m_beamModel;
public:

    VisFemNode ();
    virtual ~VisFemNode ();

    IvfClassInfo("VisFemNode",Node);

    void refresh();

    FemNode* getFemNode();
    void setFemNode(FemNode* node);

    void setBeamModel(VisBeamModel* model);
    void setDirectRefresh(bool flag);
    void getDisplacedPosition(double &x, double &y, double &z);

    virtual void setPosition (const double x, const double y, const double z);
    virtual void setPosition (ivf::Shape* shape);
     virtual void setPositionVec (ivf::Vec3d* point);
protected:
    virtual void doCreateGeometry();
};
#endif
