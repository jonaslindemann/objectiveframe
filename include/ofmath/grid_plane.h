#pragma once

#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/intersect.hpp>

namespace ofmath {

class GridPlane {
private:
    glm::vec3 m_origin;
    glm::vec3 m_direction;
    glm::vec3 m_planeOrig;
    glm::vec3 m_planeNormal;
    double m_intersectDist;
    double m_snapDistance;
    bool m_snap;
    double snap(double original, int numerator, int denominator);
    double snap(double original);
    glm::vec3 snap(glm::vec3 v);

public:
    GridPlane();
    virtual ~GridPlane();

    void setOrigin(glm::vec3 origin);
    void setOrigin(double *v);
    void setOrigin(double x, double y, double z);
    void setDirection(glm::vec3 direction);
    void setDirection(double *v);
    void setDirection(double x, double y, double z);
    void setPlaneOrigin(glm::vec3 planeOrigin);
    void setPlaneOrigin(double *v);
    void setPlaneOrigin(double x, double y, double z);
    void setPlaneNormal(glm::vec3 planeNormal);
    void setPlaneNormal(double *v);
    void setPlaneNormal(double x, double y, double z);

    glm::vec3 direction();
    glm::vec3 origin();
    glm::vec3 planeNormal();
    glm::vec3 planeOrigin();

    void setSnapDistance(double dist);
    double snapDistance();

    void setSnap(bool flag);
    void enableSnap();
    void disableSnap();
    bool isSnapEnabled();

    glm::vec3 intersect(glm::vec3 v);
    glm::vec3 intersect(double *v);
};

} // namespace ofmath
