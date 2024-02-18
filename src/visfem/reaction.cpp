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

    m_arrowMaterial = ivf::Material::create();
    m_arrowMaterial->setDiffuseColor(1.0f, 0.5f, 0.0f, 1.0f);

    m_reactionX->setMaterial(m_arrowMaterial);
    m_reactionY->setMaterial(m_arrowMaterial);
    m_reactionZ->setMaterial(m_arrowMaterial);

    this->addChild(m_reactionX);
    this->addChild(m_reactionY);
    this->addChild(m_reactionZ);

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

void vfem::Reaction::setArrowMaterial(ivf::Material *material)
{
    m_arrowMaterial = material;
    m_reactionX->setMaterial(material);
    m_reactionY->setMaterial(material);
    m_reactionZ->setMaterial(material);
}
