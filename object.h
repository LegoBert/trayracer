#pragma once
#include "ray.h"
#include "color.h"
#include <float.h>
#include <string>
#include <memory>

class Object;

//------------------------------------------------------------------------------

struct HitResult
{
    vec3 hitPoint;
    vec3 normal;
    Object* object = nullptr;
    float t = FLT_MAX;
};

class Object
{
public:
    Object() 
    {
        static unsigned long long idCounter = 0;
        id = idCounter++;
    }
    virtual ~Object() {}
    virtual bool Intersect(Ray ray, float maxDist, HitResult& hit) { return {}; };
    virtual bool hit_sphere(Ray& r) { return false; };
    virtual Color GetColor() = 0;
    virtual Ray ScatterRay(Ray ray, vec3 point, vec3 normal) { return Ray({ 0,0,0 }, {1,1,1}); };
    unsigned long long GetId() { return this->id; }
private:
    unsigned long long id;
};