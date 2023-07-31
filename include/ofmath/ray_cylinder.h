#pragma once

#include <glm/glm.hpp>

namespace ofmath {

bool intersectRayOrientedCylinder(glm::vec3 ray_origin, glm::vec3 direction, float radius, float height,
                                  glm::vec3 cylinder_position, glm::vec3 cylinder_axis, glm::vec3 &pos,
                                  glm::vec3 &normal);

}
