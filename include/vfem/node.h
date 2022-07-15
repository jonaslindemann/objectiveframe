#pragma once

#include <ofem/node.h>

#include <ivfmath/Vec3d.h>
#include <ivf/Node.h>

#include <vfem/beam_model.h>

namespace vfem {

IvfSmartPointer(Node);

// public class: VisFemNode
class Node : public ivf::Node {
private:
    ofem::Node* m_femNode;
    bool m_directRefresh;
    vfem::BeamModel* m_beamModel;
public:

    Node ();
    virtual ~Node ();

    IvfClassInfo("vfem::Node",ivf::Node);

    void refresh();

    ofem::Node* getFemNode();
    void setFemNode(ofem::Node* node);

    void setBeamModel(vfem::BeamModel* model);
    void setDirectRefresh(bool flag);
    void getDisplacedPosition(double &x, double &y, double &z);

    virtual void setPosition (const double x, const double y, const double z);
    virtual void setPosition (ivf::Shape* shape);
    virtual void setPositionVec (ivf::Vec3d* point);
protected:
    virtual void doCreateGeometry();
};
}