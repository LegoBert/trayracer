#include <stdio.h>
#include "window.h"
#include "vec3.h"
#include "raytracer.h"
#include "sphere.h"
#include <iostream>
#include <chrono>

#define degtorad(angle) angle * MPI / 180

struct Random {
private:
    uint32_t  seed;
public:
    Random(unsigned int seed)
    {
        this->seed = seed;
    }

    uint32_t Get()
    {
        seed = 1664525 * seed + 1013904223;
        return seed;
    }

    float GetFloat() {
        return static_cast<float>(Get()) / UINT32_MAX;
    }
};

#define no_gl 0
#if no_gl
int main(int argc, char* argv[])
{
    if (argc != 5) {
        std::cout << "Usage: " << argv[0] << " <width> <height> <raysPerPixel> <numOfSpheres>" << std::endl;
        return 1;
    }
    int w = atoi(argv[1]);
    int h = atoi(argv[2]);
    int raysPerPixel = atoi(argv[3]);
    int maxBounces = 5;
    int numOfSpheres = atoi(argv[4]);

    // Setting seed
    Random random(123456);

    std::vector<Color> framebuffer;
    framebuffer.resize(w * h);

    Raytracer rt = Raytracer(w, h, framebuffer, raysPerPixel, maxBounces);

    // Create some objects
    Material* mat = new Material();
    mat->type = "Lambertian";
    mat->color = { 0.5,0.5,0.5 };
    mat->roughness = 0.3;
    Sphere* ground = new Sphere(1000, { 0,-1000, -1 }, mat);
    rt.AddObject(ground);

    //Creating spheres
    for (int i = 0; i < numOfSpheres; i++)
    {
        Material* mat = new Material();
        mat->type = "Lambertian";
        float r = random.GetFloat();
        float g = random.GetFloat();
        float b = random.GetFloat();
        mat->color = { r,g,b };
        mat->roughness = random.GetFloat();
        const float span = 10.0f;
        Sphere* ground = new Sphere(
            random.GetFloat() * 0.7f + 0.2f,
            {
                random.GetFloat()* span,
                random.GetFloat()* span + 0.2f,
                random.GetFloat()* span
            },
            mat);
        rt.AddObject(ground);
    }

    // camera
    bool resetFramebuffer = false;
    vec3 camPos = { 0,1.0f,10.0f };
    vec3 moveDir = { 0,0,0 };
    float pitch = 0;
    float yaw = 0;
    float oldx = 0;
    float oldy = 0;
    float rotx = 0;
    float roty = 0;

    // number of accumulated frames
    int frameIndex = 0;

    std::vector<Color> framebufferCopy;
    framebufferCopy.resize(w * h);


    for (int i = 0; i < 1000; i++) {
        auto start = std::chrono::high_resolution_clock::now();

        resetFramebuffer = false;
        moveDir = { 0,0,0 };
        pitch = 0;
        yaw = 0;

        rotx -= pitch;
        roty -= yaw;

        moveDir = normalize(moveDir);

        mat4 xMat = (rotationx(rotx));
        mat4 yMat = (rotationy(roty));
        mat4 cameraTransform = multiply(yMat, xMat);

        camPos = camPos + transform(moveDir * 0.2f, cameraTransform);

        cameraTransform.m30 = camPos.x;
        cameraTransform.m31 = camPos.y;
        cameraTransform.m32 = camPos.z;

        rt.SetViewMatrix(cameraTransform);

        if (resetFramebuffer)
        {
            rt.Clear();
            frameIndex = 0;
        }

        rt.Raytrace();
        frameIndex++;

        // Get the average distribution of all samples
        {
            size_t p = 0;
            for (Color const& pixel : framebuffer)
            {
                framebufferCopy[p] = pixel;
                framebufferCopy[p].r /= frameIndex;
                framebufferCopy[p].g /= frameIndex;
                framebufferCopy[p].b /= frameIndex;
                p++;
            }
        }

        auto stop = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
        // Total time
        std::cout << "Time taken by code: " << duration.count() << " microseconds" << std::endl;
        // MRays / s
        int total_rays = w * h * raysPerPixel;
        float mray_per_sec = total_rays / float(duration.count());
        std::cout << mray_per_sec << " MRays / s" << std::endl;
    }

    return 0;
}
#else
int main()
{ 
    Random random(123456);

    Display::Window wnd;
    
    wnd.SetTitle("TrayRacer");
    
    if (!wnd.Open())
        return 1;

    std::vector<Color> framebuffer;

    /*const unsigned w = 100;
    const unsigned h = 100;*/
    const unsigned w = 1024;
    const unsigned h = 768;

    framebuffer.resize(w * h);
    
    int raysPerPixel = 1;
    int maxBounces = 5;

    Raytracer rt = Raytracer(w, h, framebuffer, raysPerPixel, maxBounces);

    // Create some objects
    Material* mat = new Material();
    mat->type = "Lambertian";
    mat->color = { 0.5,0.5,0.5 };
    mat->roughness = 0.3;
    Sphere* ground = new Sphere(1000, { 0,-1000, -1 }, mat);
    rt.AddObject(ground);

    for (int it = 0; it < 12; it++)
    {
        {
            Material* mat = new Material();
            mat->type = "Lambertian";
            float r = random.GetFloat();
            float g = random.GetFloat();
            float b = random.GetFloat();
            mat->color = { r,g,b };
            mat->roughness = random.GetFloat();
            const float span = 10.0f;
            Sphere* ground = new Sphere(
                random.GetFloat() * 0.7f + 0.2f,
                {
                    random.GetFloat()* span,
                    random.GetFloat()* span + 0.2f,
                    random.GetFloat()* span
                },
                mat);
            rt.AddObject(ground);
        } {
            Material* mat = new Material();
            mat->type = "Conductor";
            float r = random.GetFloat();
            float g = random.GetFloat();
            float b = random.GetFloat();
            mat->color = { r,g,b };
            mat->roughness = random.GetFloat();
            const float span = 30.0f;
            Sphere* ground = new Sphere(
                random.GetFloat() * 0.7f + 0.2f,
                {
                    random.GetFloat()* span,
                    random.GetFloat()* span + 0.2f,
                    random.GetFloat()* span
                },
                mat);
            rt.AddObject(ground);
        } {
            Material* mat = new Material();
            mat->type = "Dielectric";
            float r = random.GetFloat();
            float g = random.GetFloat();
            float b = random.GetFloat();
            mat->color = { r,g,b };
            mat->roughness = random.GetFloat();
            mat->refractionIndex = 1.65;
            const float span = 25.0f;
            Sphere* ground = new Sphere(
                random.GetFloat() * 0.7f + 0.2f,
                {
                    random.GetFloat()* span,
                    random.GetFloat()* span + 0.2f,
                    random.GetFloat()* span
                },
                mat);
            rt.AddObject(ground);
        }
    }
    
    bool exit = false;

    // camera
    bool resetFramebuffer = false;
    vec3 camPos = { 0,1.0f,10.0f };
    vec3 moveDir = { 0,0,0 };

    wnd.SetKeyPressFunction([&exit, &moveDir, &resetFramebuffer](int key, int scancode, int action, int mods)
    {
        switch (key)
        {
        case GLFW_KEY_ESCAPE:
            exit = true;
            break;
        case GLFW_KEY_W:
            moveDir.z -= 1.0f;
            resetFramebuffer |= true;
            break;
        case GLFW_KEY_S:
            moveDir.z += 1.0f;
            resetFramebuffer |= true;
            break;
        case GLFW_KEY_A:
            moveDir.x -= 1.0f;
            resetFramebuffer |= true;
            break;
        case GLFW_KEY_D:
            moveDir.x += 1.0f;
            resetFramebuffer |= true;
            break;
        case GLFW_KEY_SPACE:
            moveDir.y += 1.0f;
            resetFramebuffer |= true;
            break;
        case GLFW_KEY_LEFT_CONTROL:
            moveDir.y -= 1.0f;
            resetFramebuffer |= true;
            break;
        default:
            break;
        }
    });

    float pitch = 0;
    float yaw = 0;
    float oldx = 0;
    float oldy = 0;

    wnd.SetMouseMoveFunction([&pitch, &yaw, &oldx, &oldy, &resetFramebuffer](double x, double y)
    {
        x *= -0.1;
        y *= -0.1;
        yaw = x - oldx;
        pitch = y - oldy;
        resetFramebuffer |= true;
        oldx = x;
        oldy = y;
    });

    float rotx = 0;
    float roty = 0;

    // number of accumulated frames
    int frameIndex = 0;

    std::vector<Color> framebufferCopy;
    framebufferCopy.resize(w * h);

    // rendering loop
    while (wnd.IsOpen() && !exit)
    {
        resetFramebuffer = false;
        moveDir = {0,0,0};
        pitch = 0;
        yaw = 0;

        // poll input
        wnd.Update();

        rotx -= pitch;
        roty -= yaw;

        moveDir = normalize(moveDir);

        mat4 xMat = (rotationx(rotx));
        mat4 yMat = (rotationy(roty));
        mat4 cameraTransform = multiply(yMat, xMat);

        camPos = camPos + transform(moveDir * 0.2f, cameraTransform);
        
        cameraTransform.m30 = camPos.x;
        cameraTransform.m31 = camPos.y;
        cameraTransform.m32 = camPos.z;

        rt.SetViewMatrix(cameraTransform);
        
        if (resetFramebuffer)
        {
            rt.Clear();
            frameIndex = 0;
        }

        rt.Raytrace();
        frameIndex++;

        // Get the average distribution of all samples
        {
            size_t p = 0;
            for (Color const& pixel : framebuffer)
            {
                framebufferCopy[p] = pixel;
                framebufferCopy[p].r /= frameIndex;
                framebufferCopy[p].g /= frameIndex;
                framebufferCopy[p].b /= frameIndex;
                p++;
            }
        }

        glClearColor(0, 0, 0, 1.0);
        glClear( GL_COLOR_BUFFER_BIT );

        wnd.Blit((float*)&framebufferCopy[0], w, h);
        wnd.SwapBuffers();
    }

    if (wnd.IsOpen())
        wnd.Close();

    return 0;
}
#endif