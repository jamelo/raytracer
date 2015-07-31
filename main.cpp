#include "config.h"

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>

#include <boost/timer/timer.hpp>
#include <SFML/Graphics.hpp>
#include <QtGui/QApplication>

#include <Camera.hpp>
#include <Image.hpp>
#include <Raytracer.hpp>
#include "RaytracerWindow.h"
#include <ThreadPool.hpp>

//TODO: possible optimization: Use template singletons to store lists of objects of a particular type.
//TODO: a new template will get instantiated for each type when the singleton is created. Otherwise the object
//TODO: will just be added to the list. Types need to be registered for static dispatch. The effect is precomputed
//TODO: dynamic dispatch.

int main(__attribute__((unused)) int argc, __attribute__((unused)) char** argv)
{
    //Camera camera(1280, 720, {1.4, 1.7, 5}, {-1.5, -2.0, -5}, 1.3, {0.3, 1, 0});

    //SurfaceDescription planeSurface({1, 1, 1}, 0, 0, 0, 5.0);
    //SurfaceDescription sphereSurface({0.6, 0.8, 1}, 1);

    //std::vector<std::unique_ptr<Shape>> shapes;
    //shapes.push_back(std::unique_ptr<Shape>(new Plane({0, 0, -3}, {1, 0, 0}, {0, 1, 0}, planeSurface)));
    //shapes.push_back(std::unique_ptr<Shape>(new Sphere({0, 0, 1}, {0, 1, 0}, 1.0, sphereSurface)));
    //shapes.push_back(std::unique_ptr<Shape>(new Chmutov({0, 0, 1}, {0, 1, 0}, 1.0, 0.5, sphereSurface)));

    Camera camera(1280, 720, {0, 0, -6}, {0, 0, 1}, 0.75);

    SurfaceDescription redWall({1, 0, 0}, 1, 0, 0, 0);
    SurfaceDescription greenWall({0, 1, 0}, 1, 0, 0, 0);
    SurfaceDescription whiteWall({1, 1, 1}, 1, 0, 0, 0);
    SurfaceDescription ceiling({1, 1, 1}, 0, 0, 0, 4);
    SurfaceDescription reflectiveSphere({1, 1, 1}, 0, 0.8, 0, 0);

    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::unique_ptr<Shape>(new Plane({-4, 0, 0}, {0, 1, 0}, {0, 0, 1}, redWall)));
    shapes.push_back(std::unique_ptr<Shape>(new Plane({+4, 0, 0}, {0, -1, 0}, {0, 0, 1}, greenWall)));
    shapes.push_back(std::unique_ptr<Shape>(new Plane({0, +3, 0}, {1, 0, 0}, {0, 0, 1}, whiteWall)));
    shapes.push_back(std::unique_ptr<Shape>(new Plane({0, -3, 0}, {-1, 0, 0}, {0, 0, 1}, whiteWall)));
    shapes.push_back(std::unique_ptr<Shape>(new Plane({0, 0, +3}, {-1, 0, 0}, {0, 1, 0}, whiteWall)));
    shapes.push_back(std::unique_ptr<Shape>(new Plane({0, 0, -10}, {1, 0, 0}, {0, 1, 0}, whiteWall)));
    //shapes.push_back(std::unique_ptr<Shape>(new Sphere({0, 0, 0}, {0, 1, 0}, 1.6, reflectiveSphere)));
    shapes.push_back(std::unique_ptr<Shape>(new Sphere({0, 0, 0}, {0, 1, 0}, 0.5, ceiling)));
    //shapes.push_back(std::unique_ptr<Shape>(new Sphere({-2, -2, 0}, {0, 1, 0}, 1.0, reflectiveSphere)));
    //shapes.push_back(std::unique_ptr<Shape>(new Chmutov({2, 0, 0}, {0, 1, 0}, 3, 1, reflectiveSphere)));

    std::vector<std::unique_ptr<LightSource>> lights;

    Scene scene(std::move(camera), std::move(shapes), std::move(lights));

    ThreadPool pool;

    boost::timer::auto_cpu_timer t;
    auto result = scene.render(pool);
    auto& task = std::get<0>(result);
    auto& image = std::get<1>(result);

    task.setCompleteCallback([&image, &t](bool success) {
        t.stop();

        if (success) {
            std::cout << "Render complete." << std::endl;
            image->save("render.png");
        } else {
            std::cout << "Render cancelled." << std::endl;
        }

        t.report();
    });

    task.setProblemCallback([&image](Problem problem) {
        size_t x = 0;
        size_t y = problem[0];
    });

    QApplication app(argc, argv);
    RaytracerWindow mainWindow;
    mainWindow.show();
    app.exec();

    task.wait();

    return 0;
}
