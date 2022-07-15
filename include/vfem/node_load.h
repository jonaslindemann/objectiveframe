#pragma once

#include <ofem/beam_node_load.h>

#include <ivf/Shape.h>
#include <ivf/ExtrArrow.h>

#include <vfem/color_table.h>
#include <vfem/beam_model.h>

namespace vfem {

IvfSmartPointer(NodeLoad);

// public class: VisFemNodeLoad
class NodeLoad : public ivf::Shape {
private:
    ofem::BeamNodeLoad* m_nodeLoad;
    std::vector<ivf::ExtrArrowPtr> m_arrow;
    ivf::MaterialPtr m_arrowMaterial;
    vfem::BeamModel* m_beamModel;

    void initArrow();
public:
    NodeLoad ();
    virtual ~NodeLoad ();

    // Class info

    IvfClassInfo("vfem::NodeLoad",ivf::Shape);

    // Get/set methods

    void setBeamModel(vfem::BeamModel* model);
    void setNodeLoad(ofem::BeamNodeLoad* nodeLoad);

    // Methods

    void refresh();

protected:
    virtual void doCreateSelect();
    virtual void doCreateGeometry();
};
}