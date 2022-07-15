#include <vfem/bc.h>

#include <ivf/Cone.h>

using namespace ivf;
using namespace vfem;


BC::BC()
{
    initBC();
}

BC::~BC()
{

}

void BC::initBC()
{
    double nodeSize;

    if (m_beamModel!=nullptr)
        nodeSize = m_beamModel->getNodeSize();
    else
        nodeSize = 1.0;

    m_dispX = Transform::create();
    m_dispY = Transform::create();
    m_dispZ = Transform::create();

    m_dispCone = Cone::create();
    m_dispCone->setBottomRadius(nodeSize*0.5);
    m_dispCone->setHeight(nodeSize);
    m_dispCone->setPosition(0.0,-nodeSize,0.0);

    m_dispX->addChild(m_dispCone);
    m_dispX->setRotationQuat(0.0, 0.0, 1.0, -90.0);
    m_dispY->addChild(m_dispCone);
    m_dispZ->addChild(m_dispCone);
    m_dispZ->setRotationQuat(1.0, 0.0, 0.0, 90.0);

    this->addChild(m_dispX);
    this->addChild(m_dispY);
    this->addChild(m_dispZ);

    m_rotX = Transform::create();
    m_rotY = Transform::create();
    m_rotZ = Transform::create();

    m_rotCone1 = Cone::create();
    m_rotCone1->setBottomRadius(nodeSize*0.5);
    m_rotCone1->setHeight(nodeSize);
    m_rotCone1->setPosition(0.0,-nodeSize*2.2,0.0);

    m_rotCone2 = Cone::create();
    m_rotCone2->setBottomRadius(nodeSize*0.5);
    m_rotCone2->setHeight(nodeSize);
    m_rotCone2->setPosition(0.0,-nodeSize*3.0,0.0);

    m_rotX->addChild(m_rotCone1);
    m_rotX->addChild(m_rotCone2);
    m_rotX->setRotationQuat(0.0, 0.0, 1.0, -90.0);
    m_rotY->addChild(m_rotCone1);
    m_rotY->addChild(m_rotCone2);
    m_rotZ->addChild(m_rotCone1);
    m_rotZ->addChild(m_rotCone2);
    m_rotZ->setRotationQuat(1.0, 0.0, 0.0, 90.0);

    this->addChild(m_rotX);
    this->addChild(m_rotY);
    this->addChild(m_rotZ);

    this->setPosition(0.0,3.0,0.0);
}

void BC::refresh()
{
    double nodeSize;

    if (m_beamModel!=nullptr)
        nodeSize = m_beamModel->getNodeSize();
    else
        nodeSize = 1.0;

    m_dispCone->setBottomRadius(nodeSize*0.5);
    m_dispCone->setHeight(nodeSize);
    m_dispCone->setPosition(0.0,-nodeSize,0.0);
    m_rotCone1->setBottomRadius(nodeSize*0.5);
    m_rotCone1->setHeight(nodeSize);
    m_rotCone1->setPosition(0.0,-nodeSize*2.2,0.0);
    m_rotCone2->setBottomRadius(nodeSize*0.5);
    m_rotCone2->setHeight(nodeSize);
    m_rotCone2->setPosition(0.0,-nodeSize*3.0,0.0);
}

void BC::prescribe(int dof, bool prescribed)
{
    if ((dof>=1)&&(dof<=6))
    {
        switch (dof) {
        case 1:
            if (prescribed)
                m_dispX->setState(Shape::OS_ON);
            else
                m_dispX->setState(Shape::OS_OFF);
            break;
        case 2:
            if (prescribed)
                m_dispY->setState(Shape::OS_ON);
            else
                m_dispY->setState(Shape::OS_OFF);
            break;
        case 3:
            if (prescribed)
                m_dispZ->setState(Shape::OS_ON);
            else
                m_dispZ->setState(Shape::OS_OFF);
            break;
        case 4:
            if (prescribed)
                m_rotX->setState(Shape::OS_ON);
            else
                m_rotX->setState(Shape::OS_OFF);
            break;
        case 5:
            if (prescribed)
                m_rotY->setState(Shape::OS_ON);
            else
                m_rotY->setState(Shape::OS_OFF);
            break;
        case 6:
            if (prescribed)
                m_rotZ->setState(Shape::OS_ON);
            else
                m_rotZ->setState(Shape::OS_OFF);
            break;
        default:
            break;
        }
    }
}

void BC::unprescribeAll()
{
    int i;
    for (i=1; i<=6; i++)
        prescribe(i,false);
}

void BC::setBeamModel(BeamModel*model)
{
    m_beamModel = model;
    refresh();
}
