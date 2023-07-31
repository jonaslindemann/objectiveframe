#pragma once

#include <ofem/beam_load.h>

#include <ivf/ExtrArrow.h>
#include <ivf/Material.h>
#include <ivf/QuadSet.h>
#include <ivf/Shape.h>

#include <vfem/beam_model.h>
#include <vfem/color_table.h>

namespace vfem {

IvfSmartPointer(VisBeamLoad);

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

    void setColorTable(vfem::ColorTable *colorTable);
    void setBeamModel(vfem::BeamModel *model);

    void setBeamLoad(ofem::BeamLoad *load);

    virtual void refresh() override;

protected:
    virtual void doCreateSelect() override;
    virtual void doCreateGeometry() override;
};
} // namespace vfem
