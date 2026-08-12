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

/**
 * One smoothing pass over an adjacency list.
 *
 * Each movable point moves a fraction `factor` of the way towards the weighted
 * average of its neighbours. A positive factor shrinks the cloud, a negative
 * one inflates it - alternating the two is what makes Taubin smoothing keep
 * the overall size (see taubinSmooth).
 *
 * All new positions are computed from the old ones, so the result does not
 * depend on the order the points happen to be stored in.
 *
 * \param movable       points with false are held fixed, but still act as
 *                      neighbours for the points that move
 * \param lengthWeighted weight neighbours by 1/distance instead of uniformly.
 *                      Uniform pulls a point to the centroid of its
 *                      neighbours, which evens out member lengths; inverse
 *                      distance is gentler on irregular spacing.
 */
void laplacianPass(std::vector<glm::dvec3> &pts, const std::vector<std::vector<int>> &adjacency,
                   const std::vector<bool> &movable, double factor, bool lengthWeighted);

/**
 * Taubin smoothing: `iterations` shrink/inflate pairs.
 *
 * Plain Laplacian smoothing (mu == 0) pulls a free standing structure towards
 * its own centroid, which on a truss changes the span rather than just
 * tidying it. The second pass with a slightly larger negative factor pushes
 * back out and cancels most of that.
 *
 * Typical values are lambda 0.5 and mu -0.53. mu == 0 runs plain Laplacian.
 */
void taubinSmooth(std::vector<glm::dvec3> &pts, const std::vector<std::vector<int>> &adjacency,
                  const std::vector<bool> &movable, int iterations, double lambda, double mu, bool lengthWeighted);

} // namespace ofmath
