#include <ofmath/geom_ops.h>

#include <algorithm>
#include <cmath>

namespace ofmath {

glm::dvec3 centroid(const std::vector<glm::dvec3> &pts)
{
    if (pts.empty())
        return glm::dvec3(0.0);

    glm::dvec3 sum(0.0);

    for (const auto &p : pts)
        sum += p;

    return sum / double(pts.size());
}

void boundingBox(const std::vector<glm::dvec3> &pts, glm::dvec3 &lo, glm::dvec3 &hi)
{
    if (pts.empty())
        return;

    lo = pts[0];
    hi = pts[0];

    for (const auto &p : pts)
    {
        lo = glm::min(lo, p);
        hi = glm::max(hi, p);
    }
}

glm::dmat4 translationMatrix(const glm::dvec3 &delta)
{
    glm::dmat4 m(1.0);

    m[3] = glm::dvec4(delta, 1.0);

    return m;
}

glm::dmat4 scaleMatrix(const glm::dvec3 &factors, const glm::dvec3 &origin)
{
    glm::dmat4 m(1.0);

    m[0][0] = factors.x;
    m[1][1] = factors.y;
    m[2][2] = factors.z;

    // Move the origin to the world origin, scale, move it back

    m[3] = glm::dvec4(origin - factors * origin, 1.0);

    return m;
}

glm::dmat4 rotationMatrix(const glm::dvec3 &axis, double angleRad, const glm::dvec3 &origin)
{
    double length = glm::length(axis);

    if (length < 1e-12)
        return glm::dmat4(1.0);

    glm::dvec3 u = axis / length;

    double c = std::cos(angleRad);
    double s = std::sin(angleRad);
    double t = 1.0 - c;

    // Rodrigues, written straight into the column major basis vectors

    glm::dmat4 m(1.0);

    m[0] = glm::dvec4(t * u.x * u.x + c, t * u.x * u.y + s * u.z, t * u.x * u.z - s * u.y, 0.0);
    m[1] = glm::dvec4(t * u.x * u.y - s * u.z, t * u.y * u.y + c, t * u.y * u.z + s * u.x, 0.0);
    m[2] = glm::dvec4(t * u.x * u.z + s * u.y, t * u.y * u.z - s * u.x, t * u.z * u.z + c, 0.0);

    glm::dvec3 rotatedOrigin = glm::dvec3(m * glm::dvec4(origin, 1.0));
    m[3] = glm::dvec4(origin - rotatedOrigin, 1.0);

    return m;
}

glm::dmat4 mirrorMatrix(const glm::dvec3 &planePoint, const glm::dvec3 &planeNormal)
{
    double length = glm::length(planeNormal);

    if (length < 1e-12)
        return glm::dmat4(1.0);

    glm::dvec3 n = planeNormal / length;

    glm::dmat4 m(1.0);

    m[0] = glm::dvec4(1.0 - 2.0 * n.x * n.x, -2.0 * n.x * n.y, -2.0 * n.x * n.z, 0.0);
    m[1] = glm::dvec4(-2.0 * n.x * n.y, 1.0 - 2.0 * n.y * n.y, -2.0 * n.y * n.z, 0.0);
    m[2] = glm::dvec4(-2.0 * n.x * n.z, -2.0 * n.y * n.z, 1.0 - 2.0 * n.z * n.z, 0.0);

    // The plane does not pass through the world origin in general

    m[3] = glm::dvec4(2.0 * glm::dot(planePoint, n) * n, 1.0);

    return m;
}

void applyTaper(std::vector<glm::dvec3> &pts, int axis, double s0, double s1, const glm::dvec3 &origin)
{
    if (pts.empty() || axis < 0 || axis > 2)
        return;

    glm::dvec3 lo(0.0), hi(0.0);
    boundingBox(pts, lo, hi);

    double span = hi[axis] - lo[axis];

    // A selection with no extent along the axis has no low and high end to
    // interpolate between, so the taper is not defined. Fall back on the mean
    // factor, which is the limit the interpolation approaches.

    bool degenerate = std::abs(span) < 1e-12;

    for (auto &p : pts)
    {
        double t = degenerate ? 0.5 : (p[axis] - lo[axis]) / span;
        double s = s0 + t * (s1 - s0);

        for (int i = 0; i < 3; i++)
        {
            if (i == axis)
                continue;

            p[i] = origin[i] + (p[i] - origin[i]) * s;
        }
    }
}

glm::dvec3 transformPoint(const glm::dmat4 &m, const glm::dvec3 &p)
{
    return glm::dvec3(m * glm::dvec4(p, 1.0));
}

} // namespace ofmath
