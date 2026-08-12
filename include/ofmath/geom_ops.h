#pragma once

#include <vector>

#include <glm/glm.hpp>

/**
 * Geometry operations on plain point clouds.
 *
 * These know nothing about the FEM model or the scene graph, which keeps them
 * reusable and independent of what a node or a beam happens to be.
 *
 * Everything works in double precision - a transform applied repeatedly to a
 * model is exactly the case where float drift becomes visible.
 */
namespace ofmath {

glm::dvec3 centroid(const std::vector<glm::dvec3> &pts);

/** Axis aligned bounds. Leaves lo/hi untouched for an empty cloud. */
void boundingBox(const std::vector<glm::dvec3> &pts, glm::dvec3 &lo, glm::dvec3 &hi);

glm::dmat4 translationMatrix(const glm::dvec3 &delta);

/** Scaling about an arbitrary origin rather than about the world origin. */
glm::dmat4 scaleMatrix(const glm::dvec3 &factors, const glm::dvec3 &origin);

/**
 * Rotation of angleRad about an axis through origin.
 *
 * Returns the identity for a zero length axis, so a degenerate axis leaves the
 * model alone rather than filling it with NaN.
 */
glm::dmat4 rotationMatrix(const glm::dvec3 &axis, double angleRad, const glm::dvec3 &origin);

/** Reflection in the plane through planePoint with the given normal. */
glm::dmat4 mirrorMatrix(const glm::dvec3 &planePoint, const glm::dvec3 &planeNormal);

/**
 * Scale that varies linearly along an axis.
 *
 * Points at the low end of the bounding box are scaled by s0 and points at the
 * high end by s1, both about origin and both in the two directions
 * perpendicular to axis. The extent along axis is left alone.
 *
 * \param axis 0 for x, 1 for y, 2 for z
 */
void applyTaper(std::vector<glm::dvec3> &pts, int axis, double s0, double s1, const glm::dvec3 &origin);

glm::dvec3 transformPoint(const glm::dmat4 &m, const glm::dvec3 &p);

} // namespace ofmath
