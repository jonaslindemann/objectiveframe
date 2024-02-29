#include <vfem/reaction.h>

#include <ivf/Cone.h>

using namespace ivf;
using namespace vfem;

Reaction::Reaction()
{
    initReaction();
}

Reaction::~Reaction()
{}

void Reaction::initReaction()
{
    m_reactionX = ExtrArrow::create();
    m_reactionY = ExtrArrow::create();
    m_reactionZ = ExtrArrow::create();

    m_momentConeX = Cone::create();
    m_momentConeY = Cone::create();
    m_momentConeZ = Cone::create();

    m_momentX = GleSpiralCylinder::create();
    m_momentY = GleSpiralCylinder::create();
    m_momentZ = GleSpiralCylinder::create();

    m_forceMaterial = ivf::Material::create();
    m_forceMaterial->setDiffuseColor(1.0f, 0.5f, 0.0f, 1.0f);

    m_momentMaterial = ivf::Material::create();
    m_momentMaterial->setDiffuseColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_reactionX->setMaterial(m_forceMaterial);
    m_reactionY->setMaterial(m_forceMaterial);
    m_reactionZ->setMaterial(m_forceMaterial);

    m_momentX->setMaterial(m_momentMaterial);
    m_momentY->setMaterial(m_momentMaterial);
    m_momentZ->setMaterial(m_momentMaterial);

    m_momentConeX->setMaterial(m_momentMaterial);
    m_momentConeY->setMaterial(m_momentMaterial);
    m_momentConeZ->setMaterial(m_momentMaterial);

    m_coneXfmX = ivf::Transform::create();
    m_coneXfmY = ivf::Transform::create();
    m_coneXfmZ = ivf::Transform::create();

    m_coneXfmX->addChild(m_momentConeX);
    m_coneXfmY->addChild(m_momentConeY);
    m_coneXfmZ->addChild(m_momentConeZ);

    this->addChild(m_reactionX);
    this->addChild(m_reactionY);
    this->addChild(m_reactionZ);

    this->addChild(m_momentX);
    this->addChild(m_momentY);
    this->addChild(m_momentZ);

    this->addChild(m_coneXfmX);
    this->addChild(m_coneXfmY);
    this->addChild(m_coneXfmZ);

    this->setPosition(0.0, 3.0, 0.0);

    m_F[0] = 0.0;
    m_F[1] = 0.0;
    m_F[2] = 0.0;

    m_M[0] = 0.0;
    m_M[1] = 0.0;
    m_M[2] = 0.0;

    this->refresh();
}

void Reaction::refresh()
{
    double nodeSize{1.0};
    double arrowSizeX{0.0};
    double arrowSizeY{0.0};
    double arrowSizeZ{0.0};
    double forceSize{1.0};

    double momentSizeX{0.0};
    double momentSizeY{0.0};
    double momentSizeZ{0.0};

    if (m_beamModel != nullptr)
    {
        nodeSize = m_beamModel->getNodeSize();
        forceSize = m_beamModel->getLoadSize();
    }

    if (m_F[0] != 0.0)
        arrowSizeX = forceSize * m_F[0] / m_maxForce;
    if (m_F[1] != 0.0)
        arrowSizeY = forceSize * m_F[1] / m_maxForce;
    if (m_F[2] != 0.0)
        arrowSizeZ = forceSize * m_F[2] / m_maxForce;

    if (m_M[0] != 0.0)
        momentSizeX = 270.0 * m_M[0] / m_maxMoment;
    if (m_M[1] != 0.0)
        momentSizeY = 270.0 * m_M[1] / m_maxMoment;
    if (m_M[2] != 0.0)
        momentSizeZ = 270.0 * m_M[2] / m_maxMoment;

    // ---- X Moments -------------------------------------

    m_momentX->setStartRadius(forceSize * 0.2);
    m_momentX->setRadius(0.03);
    m_momentX->setRadiusChangePerRev(0.0);
    m_momentX->setStartZ(0.0);
    m_momentX->setZChangePerRev(0.0);
    m_momentX->setStartAngle(0.0);
    m_momentX->setTotalSpiralAngle(momentSizeX);
    m_momentX->setRotation(0.0, 90.0, 0.0);

    m_momentConeX->setPosition(0.0, 0.0, -forceSize * 0.2);

    if (momentSizeX < 0.0)
        m_momentConeX->setRotation(0.0, 0.0, 180.0);
    else
        m_momentConeX->setRotation(0.0, 0.0, 0.0);

    m_momentConeX->setBottomRadius(0.05);
    m_momentConeX->setTopRadius(0.0);
    m_momentConeX->setHeight(0.1);

    if (momentSizeX > 0.0)
        m_coneXfmX->setRotation(-momentSizeX + 5.0, 0.0, 0.0);
    else
        m_coneXfmX->setRotation(momentSizeX - 5.0, 0.0, 0.0);

    // ---- Y Moments -------------------------------------

    m_momentY->setStartRadius(forceSize * 0.2);
    m_momentY->setRadius(0.03);
    m_momentY->setRadiusChangePerRev(0.0);
    m_momentY->setStartZ(0.0);
    m_momentY->setZChangePerRev(0.0);
    m_momentY->setStartAngle(0.0);
    m_momentY->setTotalSpiralAngle(momentSizeY);
    m_momentY->setRotation(90.0, 0.0, 0.0);

    if (momentSizeY < 0.0)
        m_momentConeY->setRotation(0.0, -90.0, 90.0);
    else
        m_momentConeY->setRotation(0.0, -90.0, -90.0);

    m_momentConeY->setPosition(forceSize * 0.2, 0.0, 0.0);
    m_momentConeY->setBottomRadius(0.05);
    m_momentConeY->setTopRadius(0.0);
    m_momentConeY->setHeight(0.1);

    if (momentSizeY > 0.0)
        m_coneXfmY->setRotation(0.0, -momentSizeY - 5.0, 0.0);
    else
        m_coneXfmY->setRotation(0.0, -momentSizeY + 5.0, 0.0);

    // ---- X Moments -------------------------------------

    m_momentZ->setStartRadius(forceSize * 0.2);
    m_momentZ->setRadius(0.03);
    m_momentZ->setRadiusChangePerRev(0.0);
    m_momentZ->setStartZ(0.0);
    m_momentZ->setZChangePerRev(0.0);
    m_momentZ->setStartAngle(0.0);
    m_momentZ->setTotalSpiralAngle(momentSizeZ);

    m_momentConeZ->setPosition(forceSize * 0.2, 0.0, 0.0);

    if (momentSizeZ < 0.0)
        m_momentConeZ->setRotation(0.0, 0.0, 180.0);
    else
        m_momentConeZ->setRotation(0.0, 0.0, 0.0);

    m_momentConeZ->setBottomRadius(0.05);
    m_momentConeZ->setTopRadius(0.0);
    m_momentConeZ->setHeight(0.1);

    if (momentSizeZ > 0.0)
        m_coneXfmZ->setRotation(0.0, 0.0, momentSizeZ + 5.0);
    else
        m_coneXfmZ->setRotation(0.0, 0.0, momentSizeZ - 5.0);

    m_reactionX->setRadius(0.05, 0.03);
    m_reactionX->setPosition(0.0, 0.0, 0.0);

    if (arrowSizeX > 0)
    {
        m_reactionX->setDirection(1.0, 0.0, 0.0);
        m_reactionX->setSize(arrowSizeX, 0.1);
        m_reactionX->setOffset(-arrowSizeX - nodeSize * 4.0);
    }
    else
    {
        m_reactionX->setDirection(-1.0, 0.0, 0.0);
        m_reactionX->setSize(abs(arrowSizeX), 0.1);
        m_reactionX->setOffset(nodeSize * 4.0);
    }

    m_reactionY->setRadius(0.05, 0.03);
    m_reactionY->setPosition(0.0, 0.0, 0.0);

    if (arrowSizeY > 0)
    {
        m_reactionY->setDirection(0.0, 1.0, 0.0);
        m_reactionY->setSize(arrowSizeY, 0.1);
        m_reactionY->setOffset(-arrowSizeY - nodeSize * 4.0);
    }
    else
    {
        m_reactionY->setDirection(0.0, -1.0, 0.0);
        m_reactionY->setSize(abs(arrowSizeY), 0.1);
        m_reactionY->setOffset(nodeSize * 4.0);
    }

    m_reactionZ->setRadius(0.05, 0.03);
    m_reactionZ->setPosition(0.0, 0.0, 0.0);

    if (arrowSizeZ > 0)
    {
        m_reactionZ->setDirection(0.0, 0.0, 1.0);
        m_reactionZ->setSize(arrowSizeZ, 0.1);
        m_reactionZ->setOffset(-arrowSizeZ - nodeSize * 4.0);
    }
    else
    {
        m_reactionZ->setDirection(0.0, 0.0, -1.0);
        m_reactionZ->setSize(abs(arrowSizeZ), 0.1);
        m_reactionZ->setOffset(nodeSize * 4.0);
    }

    if (arrowSizeX == 0.0)
        m_reactionX->setState(Shape::OS_OFF);
    else
        m_reactionX->setState(Shape::OS_ON);

    if (arrowSizeY == 0.0)
        m_reactionY->setState(Shape::OS_OFF);
    else
        m_reactionY->setState(Shape::OS_ON);

    if (arrowSizeZ == 0.0)
        m_reactionZ->setState(Shape::OS_OFF);
    else
        m_reactionZ->setState(Shape::OS_ON);

    if (momentSizeX == 0.0)
    {
        m_momentX->setState(Shape::OS_OFF);
        m_momentConeX->setState(Shape::OS_OFF);
    }
    else
    {
        m_momentX->setState(Shape::OS_ON);
        m_momentConeX->setState(Shape::OS_ON);
    }

    if (momentSizeY == 0.0)
    {
        m_momentY->setState(Shape::OS_OFF);
        m_momentConeY->setState(Shape::OS_OFF);
    }
    else
    {
        m_momentY->setState(Shape::OS_ON);
        m_momentConeY->setState(Shape::OS_ON);
    }

    if (momentSizeZ == 0.0)
    {
        m_momentZ->setState(Shape::OS_OFF);
        m_momentConeZ->setState(Shape::OS_OFF);
    }
    else
    {
        m_momentZ->setState(Shape::OS_ON);
        m_momentConeZ->setState(Shape::OS_ON);
    }
}

void Reaction::show(int dof, bool visible)
{
    if ((dof >= 1) && (dof <= 6))
    {
        switch (dof)
        {
        case 1:
            if (visible)
                m_reactionX->setState(Shape::OS_ON);
            else
                m_reactionX->setState(Shape::OS_OFF);
            break;
        case 2:
            if (visible)
                m_reactionY->setState(Shape::OS_ON);
            else
                m_reactionY->setState(Shape::OS_OFF);
            break;
        case 3:
            if (visible)
                m_reactionZ->setState(Shape::OS_ON);
            else
                m_reactionZ->setState(Shape::OS_OFF);
            break;
        case 4:
            if (visible)
                m_momentX->setState(Shape::OS_ON);
            else
                m_momentX->setState(Shape::OS_OFF);
            break;
        case 5:
            if (visible)
                m_momentY->setState(Shape::OS_ON);
            else
                m_momentY->setState(Shape::OS_OFF);
            break;
        case 6:
            if (visible)
                m_momentZ->setState(Shape::OS_ON);
            else
                m_momentZ->setState(Shape::OS_OFF);
            break;
        default:
            break;
        }
    }
}

void vfem::Reaction::setForces(double fx, double fy, double fz)
{
    m_F[0] = fx;
    m_F[1] = fy;
    m_F[2] = fz;

    this->refresh();
}

void vfem::Reaction::setMoments(double mx, double my, double mz)
{
    m_M[0] = mx;
    m_M[1] = my;
    m_M[2] = mz;

    this->refresh();
}

void vfem::Reaction::setMaxForce(double f)
{
    m_maxForce = f;

    this->refresh();
}

void vfem::Reaction::setMaxMoment(double m)
{
    m_maxMoment = m;

    this->refresh();
}

void Reaction::hideAll()
{
    int i;
    for (i = 1; i <= 6; i++)
        show(i, false);
}

void Reaction::setBeamModel(BeamModel *model)
{
    m_beamModel = model;
}

void vfem::Reaction::setForceMaterial(ivf::Material *material)
{
    m_forceMaterial = material;
    m_reactionX->setMaterial(material);
    m_reactionY->setMaterial(material);
    m_reactionZ->setMaterial(material);
}

void vfem::Reaction::setMomentMaterial(ivf::Material *material)
{
    m_momentMaterial = material;
    m_momentX->setMaterial(material);
    m_momentY->setMaterial(material);
    m_momentZ->setMaterial(material);
}
