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
    Material const* const material;

    Sphere(float radius, vec3 center, Material const* const material) : 
        radius(radius),
        center(center),
        material(material)
    {}

    ~Sphere() override {}

    Color GetColor()
    {
        return material->color;
    }

    bool Intersect(Ray ray, float maxDist, HitResult& hit) override
    {
        vec3 oc = ray.origin - this->center;
        vec3 dir = ray.dir;
        float b = dot(oc, dir);
    
        // early out if sphere is "behind" ray
        if (b > 0)
            return false;

        float a = dot(dir, dir);
        float c = dot(oc, oc) - this->radius * this->radius;
        float discriminant = b * b - a * c;

        if (discriminant > 0)
        {
            constexpr float minDist = 0.001f;
            float div = 1.0f / a;
            float sqrtDisc = sqrt(discriminant);
            float temp = (-b - sqrtDisc) * div;
            float temp2 = (-b + sqrtDisc) * div;

            if (temp < maxDist && temp > minDist)
            {
                vec3 p = ray.PointAt(temp);
                hit.hitPoint = p;
                hit.normal = (p - this->center) * (1.0f / this->radius);
                hit.t = temp;
                hit.object = this;
                return true;
            }
            if (temp2 < maxDist && temp2 > minDist)
            {
                vec3 p = ray.PointAt(temp2);
                hit.hitPoint = p;
                hit.normal = (p - this->center) * (1.0f / this->radius);
                hit.t = temp2;
                hit.object = this;
                return true;
            }
        }

        return false;
    }

    bool hit_sphere(Ray& r) override
    {
        vec3 oc = r.origin - center;
        float a = dot(r.dir, r.dir);
        float b = 2.0 * dot(oc, r.dir);
        float c = dot(oc, oc) - radius * radius;
        float discriminant = b * b - 4 * a * c;
        return (discriminant > 0);
    }

    Ray ScatterRay(Ray ray, vec3 point, vec3 normal) override
    {
        return BSDF(this->material, ray, point, normal);
    }

};