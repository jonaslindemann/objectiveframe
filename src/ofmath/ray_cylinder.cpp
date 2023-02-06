#include <ofmath/ray_cylinder.h>

#include <cmath>

#include <glm/glm.hpp>

struct Vector3
{
    float x, y, z;
    Vector3 operator+(const Vector3& v) const { return { x + v.x, y + v.y, z + v.z }; }
    Vector3 operator-(const Vector3& v) const { return { x - v.x, y - v.y, z - v.z }; }
    Vector3 operator*(float s) const { return { x * s, y * s, z * s }; }
    Vector3 operator/(float s) const { return { x / s, y / s, z / s }; }
    float dot(const Vector3& v) const { return x * v.x + y * v.y + z * v.z; }
    Vector3 cross(const Vector3& v) const
    {
        return { y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.y };
    }
    float length() const { return std::sqrt(x * x + y * y + z * z); }
    Vector3 normalize() const { return (*this) / length(); }
};

struct Ray
{
    Vector3 origin, direction;
};

bool ofmath::intersectRayOrientedCylinder(glm::vec3 ray_origin, glm::vec3 direction, float radius, float height,
    glm::vec3 cylinder_position, glm::vec3 cylinder_axis,
    glm::vec3& pos, glm::vec3& normal)
{
    glm::vec3 d = direction;
    glm::vec3 e = ray_origin;
    glm::vec3 f = e - cylinder_position;

    glm::vec3 cylinder_axis_normalized = glm::normalize(cylinder_axis);
    float a = glm::dot(d, d) - glm::dot(d, cylinder_axis_normalized) * glm::dot(d, cylinder_axis_normalized);
    float b = 2 * (glm::dot(f, d) - glm::dot(f, cylinder_axis_normalized) * glm::dot(d, cylinder_axis_normalized));
    float c = glm::dot(f, f) - glm::dot(f, cylinder_axis_normalized) * glm::dot(f, cylinder_axis_normalized) - radius * radius;

    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0)
    {
        return false;
    }

    float t = (-b - std::sqrt(discriminant)) / (2 * a);
    if (t < 0)
    {
        return false;
    }

    pos = e + d * t;
    glm::vec3 intersection_to_cylinder_bottom = pos - cylinder_position;
    float distance_along_cylinder_axis = glm::dot(intersection_to_cylinder_bottom,cylinder_axis_normalized);
    if (distance_along_cylinder_axis < 0 || distance_along_cylinder_axis > height)
    {
        return false;
    }

    normal = glm::normalize(intersection_to_cylinder_bottom - cylinder_axis_normalized * distance_along_cylinder_axis);
    return true;
}

bool intersectRayOrientedCylinder(const Ray& ray, float radius, float height,
    Vector3 cylinder_position, Vector3 cylinder_axis,
    Vector3& pos, Vector3& normal)
{
    Vector3 d = ray.direction;
    Vector3 e = ray.origin;
    Vector3 f = e - cylinder_position;

    Vector3 cylinder_axis_normalized = cylinder_axis.normalize();
    float a = d.dot(d) - d.dot(cylinder_axis_normalized) * d.dot(cylinder_axis_normalized);
    float b = 2 * (f.dot(d) - f.dot(cylinder_axis_normalized) * d.dot(cylinder_axis_normalized));
    float c = f.dot(f) - f.dot(cylinder_axis_normalized) * f.dot(cylinder_axis_normalized) - radius * radius;

    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0)
    {
        return false;
    }

    float t = (-b - std::sqrt(discriminant)) / (2 * a);
    if (t < 0)
    {
        return false;
    }

    pos = e + d * t;
    Vector3 intersection_to_cylinder_bottom = pos - cylinder_position;
    float distance_along_cylinder_axis = intersection_to_cylinder_bottom.dot(cylinder_axis_normalized);
    if (distance_along_cylinder_axis < 0 || distance_along_cylinder_axis > height)
    {
        return false;
    }

    normal = (intersection_to_cylinder_bottom - cylinder_axis_normalized * distance_along_cylinder_axis).normalize();
    return true;
}
