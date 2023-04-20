#pragma once
#include "object.h"
#include <stdlib.h>
#include <time.h>
#include "mat4.h"
#include "pbr.h"
#include "random.h"
#include "ray.h"
#include "material.h"

// returns a random point on the surface of a unit sphere
inline vec3 random_point_on_unit_sphere()
{
    float x = RandomFloatNTP();
    float y = RandomFloatNTP();
    float z = RandomFloatNTP();
    vec3 v( x, y, z );
    return normalize(v);
}

class Sphere : public Object
{
public:
    float radius;
    vec3 center;
    Material material;

    Sphere(float radius, vec3 center, Material material) : 
        radius(radius),
        center(center),
        material(material)
    {}

    ~Sphere() override {}

    Color GetColor()
    {
        return material.color;
    }

    Material GetMaterial() override
    {
        return material;
    }

    HitResult Intersect(Ray ray) override
    {
        HitResult hitInfo = {false, vec3(), vec3(), 0};
        vec3 offsetRayOrigin = ray.origin - center;

        float a = dot(ray.dir, ray.dir);
        float b = 2 * dot(offsetRayOrigin, ray.dir);
        float c = dot(offsetRayOrigin, offsetRayOrigin) - radius * radius;
        // Quadratic discriminant
        float discriminant = b * b - 4 * a * c;

        // No solution when d < 0 (ray misses sphere)
        if (discriminant >= 0) {
            // Distance to nearest intersection point (from quadratic formula)
            float dst = (-b - sqrt(discriminant)) / (2 * a);

            // Ignore intersections that occur behind the ray
            if (dst >= 0) {
                hitInfo.didHit = true;
                hitInfo.dst = dst;
                hitInfo.hitPoint = ray.origin + ray.dir * dst;
                hitInfo.normal = normalize(hitInfo.hitPoint - center);
            }
        }
        return hitInfo;
    }


    /*Ray ScatterRay(Ray ray, vec3 point, vec3 normal) override
    {
        return BSDF(this->material, ray, point, normal);
    }*/

};