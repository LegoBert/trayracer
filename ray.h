#pragma once
#include "vec3.h"

//------------------------------------------------------------------------------

class Ray
{
public:
    vec3 origin;
    vec3 dir;
    Ray(vec3 startpoint, vec3 dir) : origin(startpoint), dir(dir) {}
    ~Ray() {}
    vec3 GetOrigin() const { return origin; }
    vec3 GetDirection() const { return dir; }
    vec3 PointAt(float t)
    {
        return {origin + dir * t};
    }
};