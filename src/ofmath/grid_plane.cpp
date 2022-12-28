#include <ofmath/grid_plane.h>

ofmath::GridPlane::GridPlane()
    : m_snap { true }
    , m_snapDistance { 0.2 }
{
    m_origin.x = 0.0;
    m_origin.y = 0.0;
    m_origin.z = 0.0;

    m_direction.x = 1.0;
    m_direction.y = 1.0;
    m_direction.z = 1.0;

    m_planeOrig.x = 0.0;
    m_planeOrig.y = 0.0;
    m_planeOrig.z = 0.0;

    m_planeNormal.x = 0.0;
    m_planeNormal.y = 1.0;
    m_planeNormal.z = 0.0;
}

double ofmath::GridPlane::snap(double original, int numerator, int denominator)
{
    return round(original * denominator / numerator) * numerator / denominator;
}

double ofmath::GridPlane::snap(double original)
{
    return round(original / m_snapDistance) * m_snapDistance;
}

glm::vec3 ofmath::GridPlane::snap(glm::vec3 v)
{
    return glm::vec3(snap(v.x), snap(v.y), snap(v.z));
}

ofmath::GridPlane::~GridPlane()
{
}

void ofmath::GridPlane::setOrigin(glm::vec3 origin)
{
    m_origin = origin;
}

void ofmath::GridPlane::setOrigin(double* v)
{
    m_origin.x = float(v[0]);
    m_origin.y = float(v[1]);
    m_origin.z = float(v[2]);
}

void ofmath::GridPlane::setOrigin(double x, double y, double z)
{
    m_origin.x = float(x);
    m_origin.y = float(y);
    m_origin.z = float(z);
}

void ofmath::GridPlane::setDirection(glm::vec3 direction)
{
    m_direction = direction;
}

void ofmath::GridPlane::setDirection(double* v)
{
    m_direction.x = float(v[0]);
    m_direction.y = float(v[1]);
    m_direction.z = float(v[2]);
}

void ofmath::GridPlane::setDirection(double x, double y, double z)
{
    m_direction.x = float(x);
    m_direction.y = float(y);
    m_direction.z = float(z);
}

void ofmath::GridPlane::setPlaneOrigin(glm::vec3 planeOrigin)
{
    m_planeOrig = planeOrigin;
}

void ofmath::GridPlane::setPlaneOrigin(double* v)
{
    m_planeOrig.x = float(v[0]);
    m_planeOrig.y = float(v[1]);
    m_planeOrig.z = float(v[2]);
}

void ofmath::GridPlane::setPlaneOrigin(double x, double y, double z)
{
    m_planeOrig.x = float(x);
    m_planeOrig.y = float(y);
    m_planeOrig.z = float(z);
}

void ofmath::GridPlane::setPlaneNormal(glm::vec3 planeNormal)
{
    m_planeNormal = planeNormal;
}

void ofmath::GridPlane::setPlaneNormal(double* v)
{
    m_planeNormal.x = float(v[0]);
    m_planeNormal.y = float(v[1]);
    m_planeNormal.z = float(v[2]);
}

void ofmath::GridPlane::setPlaneNormal(double x, double y, double z)
{
    m_planeNormal.x = float(x);
    m_planeNormal.y = float(y);
    m_planeNormal.z = float(z);
}

glm::vec3 ofmath::GridPlane::direction()
{
    return m_direction;
}

glm::vec3 ofmath::GridPlane::origin()
{
    return m_origin;
}

glm::vec3 ofmath::GridPlane::planeNormal()
{
    return m_planeNormal;
}

glm::vec3 ofmath::GridPlane::planeOrigin()
{
    return m_planeOrig;
}

void ofmath::GridPlane::setSnapDistance(double dist)
{
    m_snapDistance = dist;
}

double ofmath::GridPlane::snapDistance()
{
    return m_snapDistance;
}

void ofmath::GridPlane::setSnap(bool flag)
{
    m_snap = flag;
}

void ofmath::GridPlane::enableSnap()
{
    m_snap = true;
}

void ofmath::GridPlane::disableSnap()
{
    m_snap = false;
}

bool ofmath::GridPlane::isSnapEnabled()
{
    return m_snap;
}

glm::vec3 ofmath::GridPlane::intersect(glm::vec3 v)
{
    float d;

    if (glm::intersectRayPlane(m_origin, v, m_planeOrig, m_planeNormal, d))
    {
        glm::vec3 ip = m_origin + v * d;
        return snap(ip);   
    }
    else
        return glm::vec3();
}

glm::vec3 ofmath::GridPlane::intersect(double* v)
{
    glm::vec3 vv(v[0], v[1], v[2]);
    return intersect(vv);
}
