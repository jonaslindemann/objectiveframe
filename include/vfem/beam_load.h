#pragma once

#include <ofem/beam_load.h>

#include <ivf/ExtrArrow.h>
#include <ivf/Material.h>
#include <ivf/QuadSet.h>
#include <ivf/Shape.h>

#include <vfem/beam_model.h>
#include <vfem/color_table.h>

namespace vfem {

IvfSmartPointer(BeamLoad);

class BeamLoad : public ivf::Shape {
private:
    std::vector<ivf::QuadSetPtr> m_q;
    ofem::BeamLoad *m_beamLoad;
    std::vector<ivf::ExtrArrowPtr> m_arrow;
    ivf::MaterialPtr m_extrMaterial;
    ivf::MaterialPtr m_arrowMaterial;
    vfem::ColorTable *m_colorTable;

    vfem::BeamModel *m_beamModel;

    void initExtrusion();

public:
    BeamLoad();
    virtual ~BeamLoad();

    IvfClassInfo("vfem::BeamLoad", ivf::Shape);
    IvfStdFactory(vfem::BeamLoad);

    void setColorTable(vfem::ColorTable *colorTable);
    void setBeamModel(vfem::BeamModel *model);

    void setBeamLoad(ofem::BeamLoad *load);

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
