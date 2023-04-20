#include "raytracer.h"
#include <random>

//------------------------------------------------------------------------------

Raytracer::Raytracer(unsigned w, unsigned h, std::vector<Color>& frameBuffer, unsigned rpp, unsigned bounces) :
    frameBuffer(frameBuffer),
    rpp(rpp),
    bounces(bounces),
    width(w),
    height(h)
{}

//------------------------------------------------------------------------------

void
Raytracer::Raytrace()
{
    static int leet = 1337;
    std::mt19937 generator(leet++);
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    float invWidth = 1.0f / this->width;
    float invHeight = 1.0f / this->height;

    for (int x = 0; x < this->width; ++x)
    {
        for (int y = 0; y < this->height; ++y)
        {
            Color color;
            for (int i = 0; i < this->rpp; ++i)
            {
                float u = ((float(x + dis(generator)) * invWidth) * 2.0f) - 1.0f;
                float v = ((float(y + dis(generator)) * invHeight) * 2.0f) - 1.0f;

                vec3 direction = vec3(u, v, -1.0f);
                direction = transform(direction, this->frustum);

                Ray ray(get_position(this->view), direction);
                color += this->TracePath(ray, 0);
            }

            // divide by number of samples per pixel, to get the average of the distribution
            color.r /= this->rpp;
            color.g /= this->rpp;
            color.b /= this->rpp;

            this->frameBuffer[y * this->width + x] += color;
        }
    }

}

//------------------------------------------------------------------------------
//@parameter n - the current bounce level

Color Raytracer::TracePath(Ray ray, unsigned n)
{
    /*vec3 hitpoint;
    vec3 hitnormal;

    HitResult hitInfo = Raycast(ray);
    if (hitInfo.didHit)
    {
        ray.origin = hitInfo.hitPoint;
        ray.dir = hitInfo.normal;
        if (n < this->bounces)
        {
            return hitInfo.mat->color * TracePath(ray, n + 1);
        }
        else
        {
            return { 0, 0, 0 };
        }
    }

    return this->Skybox(ray.dir);*/

    Color color;
    for (int i = 0; i <= this->bounces; i++)
    {
        HitResult hitinfo = Raycast(ray);
        if (hitinfo.didHit)
        {
            ray.origin = hitinfo.hitPoint;
            ray.dir = hitinfo.normal;
            color = color * hitinfo.mat.color;
        }
        else
        {
            break;
        }
    }
    return color;
}

//------------------------------------------------------------------------------

HitResult Raytracer::Raycast(Ray ray)
{
    HitResult closestHit;

    for (Object* obj : this->objects)
    {
        HitResult hitInfo = obj->Intersect(ray);

        if (hitInfo.didHit && hitInfo.dst < closestHit.dst) {
            closestHit = hitInfo;
            closestHit.mat = obj->GetMaterial();
        }
    }

    return closestHit;
}

//------------------------------------------------------------------------------

void
Raytracer::Clear()
{
    for (auto& color : this->frameBuffer)
    {
        color.r = 0.0f;
        color.g = 0.0f;
        color.b = 0.0f;
    }
}

//------------------------------------------------------------------------------

void
Raytracer::UpdateMatrices()
{
    mat4 inverseView = inverse(this->view);
    mat4 basis = transpose(inverseView);
    this->frustum = basis;
}

//------------------------------------------------------------------------------

Color Raytracer::Skybox(vec3 direction)
{
    float t = 0.5 * (direction.y + 1.0);
    vec3 vec = vec3(1.0, 1.0, 1.0) * (1.0 - t) + vec3(0.5, 0.7, 1.0) * t;
    return { (float)vec.x, (float)vec.y, (float)vec.z };
}