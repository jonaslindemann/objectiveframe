#pragma once

#include <ofem/beam_node_bc.h>

#include <ivf/Cone.h>
#include <ivf/ExtrArrow.h>
#include <ivf/Material.h>
#include <ivf/Shape.h>
#include <ivf/Transform.h>

#include <vfem/bc.h>
#include <vfem/beam_model.h>
#include <vfem/color_table.h>
#include <vfem/reaction.h>

namespace vfem {

IvfSmartPointer(NodeBC);

// public class: VisFemNodeBC
class NodeBC : public ivf::Shape {
private:
    ofem::BeamNodeBC *m_nodeBC;
    std::vector<vfem::BCPtr> m_bc;
    std::vector<vfem::ReactionPtr> m_reaction;
    ivf::MaterialPtr m_bcMaterial;

    vfem::BeamModel *m_beamModel;

public:
    NodeBC();
    virtual ~NodeBC();

    IvfClassInfo("vfem::NodeBC", ivf::Shape);
    IvfStdFactory(vfem::NodeBC);

    virtual void refresh() override;

    void setNodeBC(ofem::BeamNodeBC *bc);
    void setBeamModel(vfem::BeamModel *model);

protected:
    virtual void doCreateGeometry() override;
};

} // namespace vfem
