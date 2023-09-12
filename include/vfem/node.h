#pragma once

#include <ofem/node.h>

#include <ivf/Node.h>
#include <ivf/TextLabel.h>
#include <ivfmath/Vec3d.h>

#include <vfem/beam_model.h>

namespace vfem {

IvfSmartPointer(Node);

// public class: VisFemNode
class Node : public ivf::Node {
private:
    ivf::TextLabelPtr m_nodeLabel;
    ofem::Node *m_femNode;
    bool m_directRefresh;
    vfem::BeamModel *m_beamModel;

public:
    Node();
    virtual ~Node();

    IvfClassInfo("vfem::Node", ivf::Node);
    IvfStdFactory(vfem::Node);

    virtual void refresh() override;

    ofem::Node *getFemNode();
    void setFemNode(ofem::Node *node);

    void setBeamModel(vfem::BeamModel *model);
    void setDirectRefresh(bool flag);
    void getDisplacedPosition(double &x, double &y, double &z);

    ivf::TextLabel *nodeLabel();

    virtual void setPosition(const double x, const double y, const double z) override;
    virtual void setPosition(ivf::Shape *shape) override;
    virtual void setPositionVec(ivf::Vec3d *point);

protected:
    virtual void doCreateGeometry() override;
};
} // namespace vfem
