#pragma once

#include <ivf/Cone.h>
#include <ivf/ExtrArrow.h>
#include <ivf/Material.h>
#include <ivf/Transform.h>
#include <ivf/Cone.h>

#include <ivfgle/GleSpiralCylinder.h>

#include <vfem/beam_model.h>

namespace vfem {

IvfSmartPointer(Reaction);

class Reaction : public ivf::Composite {
private:
    ivf::ExtrArrowPtr m_reactionX;
    ivf::ExtrArrowPtr m_reactionY;
    ivf::ExtrArrowPtr m_reactionZ;

    ivf::GleSpiralCylinderPtr m_momentX;
    ivf::GleSpiralCylinderPtr m_momentY;
    ivf::GleSpiralCylinderPtr m_momentZ;

    ivf::ConePtr m_momentConeX;
    ivf::ConePtr m_momentConeY;
    ivf::ConePtr m_momentConeZ;

    ivf::TransformPtr m_coneXfmX;
    ivf::TransformPtr m_coneXfmY;
    ivf::TransformPtr m_coneXfmZ;

    ivf::MaterialPtr m_forceMaterial;
    ivf::MaterialPtr m_momentMaterial;

    double m_F[3];
    double m_M[3];
    double m_maxForce;
    double m_maxMoment;

    vfem::BeamModelPtr m_beamModel;

    void initReaction();

public:
    Reaction();
    virtual ~Reaction();

    IvfClassInfo("vfem::Reaction", ivf::Composite);
    IvfStdFactory(vfem::Reaction);

    void hideAll();
    void show(int dof, bool prescribed);
    void setForces(double fx, double fy, double fz);
    void setMoments(double mx, double my, double mz);
    void setMaxForce(double f);
    void setMaxMoment(double m);

    virtual void refresh() override;

    void setBeamModel(vfem::BeamModel *model);
    void setForceMaterial(ivf::Material *material);
    void setMomentMaterial(ivf::Material *material);
};

} // namespace vfem
