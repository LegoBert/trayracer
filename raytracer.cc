#include "raytracer.h"
#include <random>


//------------------------------------------------------------------------------

Raytracer::Raytracer(unsigned w, unsigned h, std::vector<Color>& frameBuffer, unsigned rpp, unsigned bounces) :
    frameBuffer(frameBuffer),
    rpp(rpp),
    bounces(bounces),
    width(w),
    height(h),
    invWidth(1.0f / this->width),
    invHeight(1.0f / this->height),
    invRpp(1.0f / this->rpp)
{}

//------------------------------------------------------------------------------

void Raytracer::Raytrace()
{
    //static int leet = 1337;
    //std::mt19937 generator(leet++);
    //std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    //std::cout << std::thread::hardware_concurrency() << std::endl;

    //for (int x = 0; x < this->width; ++x)
    //{
    //    for (int y = 0; y < this->height; ++y)
    //    {
    //        Color color;
    //        for (int i = 0; i < this->rpp; ++i)
    //        {
    //            float u = ((float(x + dis(generator)) * invWidth) * 2.0f) - 1.0f;
    //            float v = ((float(y + dis(generator)) * invHeight) * 2.0f) - 1.0f;

    //            vec3 direction = vec3(u, v, -1.0f);
    //            direction = transform(direction, this->frustum);

    //            color += TracePath(direction);
    //        }

    //        // divide by number of samples per pixel, to get the average of the distribution
    //        color *= invRpp;
    //        this->frameBuffer[y * this->width + x] += color;
    //    }
    //}
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    std::vector<std::thread> threads;
    const int num_threads = std::thread::hardware_concurrency();
    const int Hthr = height / (num_threads * 4);
    int tiles_per_thread = 1;  /*= std::max(1, height / (num_threads * 8));*/
    if (Hthr > 1) {
        tiles_per_thread = Hthr;
    }
    const int tile_size = width * tiles_per_thread;
    const int num_tiles = (height + tiles_per_thread - 1) / tiles_per_thread;

    for (int t = 0; t < num_threads; ++t)
    {
        threads.emplace_back([&, t]() {
            for (int tile = t; tile < num_tiles; tile += num_threads)
            {
                int y_start = tile * tiles_per_thread;
                int y_end = height; /*std::min(height, y_start + tiles_per_thread);*/
                if (y_start + tiles_per_thread < height) {
                    y_end = y_start + tiles_per_thread;
                }

                for (int x = 0; x < width; ++x)
                {
                    for (int y = y_start; y < y_end; ++y)
                    {
                        Color color;
                        for (int i = 0; i < rpp; ++i)
                        {
                            float u = ((float(x + dis(generator)) * invWidth) * 2.0f) - 1.0f;
                            float v = ((float(y + dis(generator)) * invHeight) * 2.0f) - 1.0f;

                            vec3 direction = vec3(u, v, -1.0f);
                            direction = transform(direction, frustum);

                            color += TracePath(direction);
                        }

                        // divide by number of samples per pixel, to get the average of the distribution
                        color *= invRpp;
                        frameBuffer[y * width + x] += color;
                    }
                }
            }
            });
    }

    for (auto& thread : threads)
    {
        thread.join();
    }

}

//------------------------------------------------------------------------------

Color Raytracer::TracePath(vec3 direction)
{
    Ray ray(origin, direction);
    Color color;
    float multiplier = 1.0f;
    HitResult hit;

    for (int i = 0; i < this->bounces; i++)
    {
        if(i != 0)
        {
            hit.object->Scatter(ray);
        }
        hit = Raycast(ray);

        if (hit.hitDst < 0.0f || hit.object == nullptr)
        {
            color += Skybox(ray.dir);
            color *= multiplier;
            break;
        }

        color += hit.object->GetColor();
        color *= multiplier;
        multiplier *= 0.5f;

        ray.origin = hit.hitPoint;
        ray.dir = hit.normal;
    }
    return color;
}

//------------------------------------------------------------------------------

HitResult Raytracer::Raycast(Ray& ray) {
    HitResult closestHit;
    HitResult hit;
    for (Object* object : this->objects)
    {
        if (object->Intersect(ray, closestHit.hitDst, hit))
        {
            if (hit.hitDst < closestHit.hitDst)
            {
                closestHit = hit;
                closestHit.object = object;
            }
        }
    }

    return closestHit;
}

//------------------------------------------------------------------------------

void Raytracer::Clear()
{
    for (auto& color : this->frameBuffer)
    {
        color.r = 0.0f;
        color.g = 0.0f;
        color.b = 0.0f;
    }
}

//------------------------------------------------------------------------------

void Raytracer::UpdateMatrices()
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
