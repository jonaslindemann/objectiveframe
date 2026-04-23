//
//  LeapFinger.cpp
//  objframe2
//
//  Created by Daniel Åkesson on 9/4/13.
//
//

#include "LeapFinger.h"

#ifdef USE_LEAP

LeapFinger::LeapFinger(CIvfFemWidget* widget)
{
    m_widget = widget;

    m_fingerPos = new CIvfVec3d;
    m_fingerDir = new CIvfVec3d;

    m_fingerPos = new CIvfVec3d;
    m_fingerDir = new CIvfVec3d;
    m_fingerShape = new CIvfCylinder();
    m_fingerShape->setRadius(0);
    m_fingerShape->setTopRadius(0.1);
    m_fingerShape->setHeight(0.7);
    //m_fingerShape->addReference();
    m_fingerShape->setState(CIvfShape::OS_OFF);
    m_widget->getScene()->getPostComposite()->addChild(m_fingerShape);

    CIvfMaterialPtr material = new CIvfMaterial();
    material->setDiffuseColor(0.3f, 1.0f, 0.3f, 0.8f);
    material->setSpecularColor(1.0f, 1.0f, 1.0f, 0.8f);
    material->setAmbientColor(0.1f, 0.3f, 0.1f, 0.8f);

    m_fingerShape->setMaterial(material);
}

LeapFinger::~LeapFinger()
{
}

void LeapFinger::fingerMove(Finger finger)
{
    m_fingerData = finger;
    m_widget->getCamera()->getAbsoluteRotation(m_alpha, m_beta);

    // Set position finger 1
    m_fingerShape->setState(CIvfShape::OS_ON);
    LeapToScene(adjustPosition(m_fingerData.stabilizedTipPosition()), m_fingerPos);
    m_fingerShape->setPosition(*m_fingerPos);

    // Set rotation finger 1
    LeapToScene(m_fingerData.direction(), m_fingerDir);
    double pitch, yaw;
    m_fingerDir->getEulerAngles(pitch, yaw);
    m_fingerShape->setRotation(-pitch - 90, 0, yaw);
}

void LeapFinger::LeapToScene(Vector leapVector, CIvfVec3d* returnVector)
{

    returnVector->setX(leapVector.x);
    returnVector->setY(leapVector.y);
    returnVector->setZ(leapVector.z);

    CIvfVec3d axis;
    axis.add(0, 1, 0);

    returnVector->rotate(axis, 90 - m_beta * RAD_TO_DEG);
}

Vector LeapFinger::adjustPosition(Vector inputVector)
{
    Vector returnVector;
    returnVector = inputVector * leapSpeed;

    // Keep in grid but not under it
    returnVector.y = inputVector.y * leapHeightSpeed;

    returnVector.y = returnVector.y - 2;

    if (returnVector.y < 0)
        returnVector.y = 0;

    return returnVector;
}

void LeapFinger::Show(bool show)
{
    if (show)
    {
        m_fingerShape->setState(CIvfShape::OS_ON);
    }
    else
    {
        m_fingerShape->setState(CIvfShape::OS_OFF);
    }
}

CIvfVec3d LeapFinger::getPosition()
{
    return m_fingerShape->getPosition();
}

void LeapFinger::setPosition(double x, double y, double z)
{
    // Adjusting for the height of the cylinder
    double adjust = -0.3;
    double d[3];
    d[0] = m_fingerData.direction().x * adjust;
    d[1] = m_fingerData.direction().y; //*adjust;
    d[2] = m_fingerData.direction().z * adjust;

    m_fingerShape->setPosition(x - d[0], y - d[1], z - d[2]);
}

#endif
