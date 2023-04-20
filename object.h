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
    float hitDst = FLT_MAX;
    vec3 hitPoint;
    vec3 normal;
    Object* object = nullptr;
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
    virtual Color GetColor() = 0;
    virtual void Scatter(Ray& ray) = 0;
    //virtual void ScatterRay(Ray& ray, vec3& point, vec3& normal) {};
    unsigned long long GetId() { return this->id; }
private:
    unsigned long long id;
};