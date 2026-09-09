#pragma once

#include <ofem/beam_node_load.h>

#include <ivf/ExtrArrow.h>
#include <ivf/Shape.h>

#include <vfem/beam_model.h>
#include <vfem/color_table.h>

namespace vfem {

IvfSmartPointer(NodeLoad);

// public class: VisFemNodeLoad
class NodeLoad : public ivf::Shape {
private:
    ofem::BeamNodeLoad *m_nodeLoad;
    std::vector<ivf::ExtrArrowPtr> m_arrow;
    ivf::MaterialPtr m_arrowMaterial;
    vfem::BeamModel *m_beamModel;

    void initArrow();

public:
    NodeLoad();
    virtual ~NodeLoad();

    // Class info

    IvfClassInfo("vfem::NodeLoad", ivf::Shape);
    IvfStdFactory(vfem::NodeLoad);

    // Get/set methods

    void setBeamModel(vfem::BeamModel *model);
    void setNodeLoad(ofem::BeamNodeLoad *nodeLoad);

    // Methods

    virtual void refresh() override;

protected:
    virtual void doCreateSelect() override;
    virtual void doCreateGeometry() override;

    /**
     * This class draws no geometry of its own -- doCreateGeometry() only renders
     * child shapes, and those have modern paths. Without saying so it inherits
     * ivf::Shape's answer of "no modern path", and RenderProfile::Core then skips
     * doCreateGeometry() entirely and takes the children with it.
     */
    virtual bool hasModernPath() override;
};
} // namespace vfem
