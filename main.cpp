#include "config.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <vector>

#include <boost/timer/timer.hpp>

#include <Camera.hpp>
#include <Image.hpp>
#include <Raytracer.hpp>

//TODO: possible optimization: Use template singletons to store lists of objects of a particular type.
//TODO: a new template will get instantiated for each type when the singleton is created. Otherwise the object
//TODO: will just be added to the list. Types need to be registered for static dispatch. The effect is precomputed
//TODO: dynamic dispatch.

int main(__attribute__((unused)) int argc, __attribute__((unused)) char** argv)
{
    boost::timer::auto_cpu_timer t;

    Camera camera(1280, 720, {1.4, 1.7, 5}, {-1.5, -2.0, -5}, 1.3, {0.3, 1, 0});

    SurfaceDescription planeSurface({1, 1, 1}, 0, 0, 0, 5.0);
    SurfaceDescription sphereSurface({0.6, 0.8, 1}, 1);

    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::unique_ptr<Shape>(new Plane({0, 0, -3}, {1, 0, 0}, {0, 1, 0}, planeSurface)));
    //shapes.push_back(std::unique_ptr<Shape>(new Sphere({0, 0, 1}, {0, 1, 0}, 1.0, sphereSurface)));
    shapes.push_back(std::unique_ptr<Shape>(new Chmutov({0, 0, 1}, {0, 1, 0}, 1.0, 0.5, sphereSurface)));

    std::vector<std::unique_ptr<LightSource>> lights;

    auto start = std::chrono::steady_clock::now();

    Scene scene(std::move(camera), std::move(shapes), std::move(lights));

    auto end = std::chrono::steady_clock::now();

    auto image = scene.render();

    image.save("render.png");

    auto time = end - start;

    /*std::printf("Done! This render took %ld h %02ld m %02ld s\n",
                std::chrono::duration_cast<std::chrono::hours>(time).count(),
                std::chrono::duration_cast<std::chrono::minutes>(time).count(),
                std::chrono::duration_cast<std::chrono::seconds>(time).count());*/

    return 0;
}
