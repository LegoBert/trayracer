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

    Material const* const GetMaterial()
    {
        return this->material;
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
                hit.hitDst = temp;
                hit.object = this;
                return true;
            }
            if (temp2 < maxDist && temp2 > minDist)
            {
                vec3 p = ray.PointAt(temp2);
                hit.hitPoint = p;
                hit.normal = (p - this->center) * (1.0f / this->radius);
                hit.hitDst = temp2;
                hit.object = this;
                return true;
            }
        }

        return false;
    }

    /*void ScatterRay(Ray& ray, vec3& point, vec3& normal) override
    {
        BSDF(this->material, ray, point, normal);
    }*/

    void Scatter(Ray& ray) override {
        Bounce(ray, this->material);
    }

    void Bounce(Ray& ray, Material const* const mat) {
        if (mat->type != Type::Dielectric)
        {
            float F0 = 0.04f;
            if (mat->type == Type::Conductor)
            {
                F0 = 0.95f;
            }
            ray.dir = mul(ray.dir, normalize(vec3(FastRandom(), FastRandom(), FastRandom()) * F0));
        }
    }
};