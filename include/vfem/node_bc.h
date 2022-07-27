#pragma once

#include <ivf/ivfconfig.h>

#include <ofem/beam_node_bc.h>

#include <ivf/Shape.h>
#include <ivf/Cone.h>
#include <ivf/Material.h>
#include <ivf/Transform.h>

#include <vfem/color_table.h>
#include <vfem/bc.h>
#include <vfem/beam_model.h>

namespace vfem {

IvfSmartPointer(CIVfFemNodeBC);

// public class: VisFemNodeBC
class NodeBC : public ivf::Shape {
private:
    ofem::BeamNodeBC* m_nodeBC;
    std::vector<vfem::BC*> m_bc;
    ivf::MaterialPtr m_bcMaterial;

    vfem::BeamModel* m_beamModel;
public:
    NodeBC();
    virtual ~NodeBC();

    IvfClassInfo("vfem::NodeBC",ivf::Shape);

    void refresh();

    void setNodeBC(ofem::BeamNodeBC* bc);
    void setBeamModel(vfem::BeamModel* model);
protected:
    virtual void doCreateGeometry();
};

}