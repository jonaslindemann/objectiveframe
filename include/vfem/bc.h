#pragma once

#include <ivf/Cone.h>
#include <ivf/Material.h>
#include <ivf/Transform.h>

#include <vfem/beam_model.h>

namespace vfem
{

IvfSmartPointer(BC);

class BC : public ivf::Composite
{
private:
    ivf::ConePtr m_dispCone;
    ivf::ConePtr m_rotCone1;
    ivf::ConePtr m_rotCone2;
    ivf::TransformPtr m_dispX;
    ivf::TransformPtr m_dispY;
    ivf::TransformPtr m_dispZ;
    ivf::TransformPtr m_rotX;
    ivf::TransformPtr m_rotY;
    ivf::TransformPtr m_rotZ;

    vfem::BeamModelPtr m_beamModel;

    void initBC();

public:
    BC();
    virtual ~BC();

    IvfClassInfo("vfem::BC", ivf::Composite);

    void unprescribeAll();
    void prescribe(int dof, bool prescribed);

    virtual void refresh() override;

    void setBeamModel(vfem::BeamModel* model);
};

}
