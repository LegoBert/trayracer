#pragma once
#include "ray.h"
#include "color.h"
#include "material.h"
#include <float.h>
#include <string>
#include <memory>

class Object;

struct HitResult
{
    bool didHit;
    vec3 hitPoint;
    vec3 normal;
    float dst;
    Material mat;
};

//------------------------------------------------------------------------------

class Object
{
public:
    Object()
    {
        static unsigned long long idCounter = 0;
        id = idCounter++;
    }
    virtual ~Object() {}
    virtual HitResult Intersect(Ray ray) { return {}; };
    virtual Color GetColor() = 0;
    virtual Material GetMaterial() = 0;
    virtual Ray ScatterRay(Ray ray, vec3 point, vec3 normal) { return Ray({ 0,0,0 }, {1,1,1}); };
    unsigned long long GetId() { return this->id; }
private:
    unsigned long long id;
};