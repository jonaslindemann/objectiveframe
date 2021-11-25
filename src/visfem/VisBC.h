#pragma once

#include <ivf/Cone.h>
#include <ivf/Material.h>
#include <ivf/Transform.h>

#include "VisBeamModel.h"

IvfSmartPointer(VisBC);

class VisBC : public ivf::Composite {
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

    VisBeamModelPtr m_beamModel;

    void initBC();
public:
    VisBC();
    virtual ~VisBC();

    IvfClassInfo("VisBC",ivf::Composite);

    void unprescribeAll();
    void prescribe(int dof, bool prescribed);

    void refresh();

    void setBeamModel(VisBeamModel* model);
};
