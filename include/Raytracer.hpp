#ifndef Raytracer_HPP
#define Raytracer_HPP

#include <random>
#include <vector>

#include <Camera.hpp>
#include <LightSource.hpp>
#include <Shape.hpp>

class Scene;

constexpr float epsilon = 1e-10;

ColourRgb<float> calculateRayColour(const Ray3& r, const Scene& scene, int recursion_depth);

class Scene
{
private:
    Camera m_camera;
    std::vector<std::unique_ptr<Shape>> m_shapes;
    std::vector<std::unique_ptr<LightSource>> m_lightSources;

public:
    /*Scene(const Camera& _cam, const std::vector<Shape>& _shapes, const std::vector<LightSource>& _lightSources) :
            m_camera(_cam),
            m_shapes(_shapes),
            m_lightSources(_lightSources) {
    }*/

    //TODO: implement a proper constructor. Why are these parameters rvalue references?
    Scene(Camera&& _cam, std::vector<std::unique_ptr<Shape>>&& _shapes, std::vector<std::unique_ptr<LightSource>>&& _lightSources) :
            m_camera(_cam),
            m_shapes(std::move(_shapes)),
            m_lightSources(std::move(_lightSources)) {
    }

    const Camera& camera() const {
        return m_camera;
    }

    const std::vector<std::unique_ptr<Shape>>& shapes() const {
        return m_shapes;
    }

    const std::vector<std::unique_ptr<LightSource>>& lightSources() const {
        return m_lightSources;
    }

    //TODO: find a more appropriate place to pass in an anti aliaser. Should it not be a property of the camera instead of a property of the scene?
    std::tuple<TaskHandle, std::shared_ptr<Image<ColourRgb<float>>>> render(ThreadPool& pool) const {
        return m_camera.render(pool, [&](const Ray3& r) { return calculateRayColour(r, *this, 10); });
    }
};

//TODO: find a more appropriate place to define pi
constexpr double pi() { return std::atan(1.0) * 4.0; }

inline Vector3 randomVectorOnUnitSphere()
{
    thread_local std::mt19937 randgen;
    thread_local std::uniform_real_distribution<float> dist(0, 1);

    float theta = 2 * pi() * dist(randgen);
    float phi = std::acos(2 * dist(randgen) - 1);

    return Vector3{std::cos(theta), std::sin(theta), std::cos(phi)};
}

//TODO: add case for transmittance
//TODO: break this function into smaller functions: one function for each surface type. sum results
ColourRgb<float> calculateRayColour(const Ray3& r, const Scene& scene, int recursion_depth)
{
    if (recursion_depth == 0) {
        return ColourRgb<float>(0, 0, 0);
    }

    ColourRgb<float> colour{0, 0, 0};

    const Shape* nearestShape = nullptr;
    double nearestIntersectionDistance = std::numeric_limits<double>::infinity();

    Ray3 rn = Ray3(r.origin(), normalize(r.direction()));

    for (auto&& shape : scene.shapes()) {
        double intersectionDistance = shape->calculateRayIntersection(rn);

        if (intersectionDistance < nearestIntersectionDistance && intersectionDistance > epsilon) {
            nearestIntersectionDistance = intersectionDistance;
            nearestShape = shape.get();
        }
    }

    if (!nearestShape) {
        return ColourRgb<float>(0, 0, 0);
    }

    Point3 pointOfIntersection = rn.origin() + rn.direction() * nearestIntersectionDistance;
    Vector3 surfaceNormal = nearestShape->calculateNormal(pointOfIntersection);

    if (surfaceNormal * rn.direction() > 0.0) {
        surfaceNormal -= surfaceNormal;
    }

    if (nearestShape->surface().difuseReflectance() > 0.0) {
        Vector3 randomVector = randomVectorOnUnitSphere();
        Vector3 nextRayDirection = (randomVector * surfaceNormal < 0.0) ? -randomVector : randomVector;

        Ray3 nextRay = Ray3(pointOfIntersection, nextRayDirection);
        double lambertianFactor = std::abs(nextRayDirection * surfaceNormal);

        colour += calculateRayColour(nextRay, scene, recursion_depth - 1) * lambertianFactor * nearestShape->surface().colour();
    }

    if (nearestShape->surface().reflectance() > 0.0) {
        Vector3 nextRayDirection = rn.direction() - 2 * (rn.direction() * surfaceNormal) * surfaceNormal;
        Ray3 nextRay = Ray3(pointOfIntersection, nextRayDirection);

        colour += calculateRayColour(nextRay, scene, recursion_depth - 1) * nearestShape->surface().colour();
    }

    if (nearestShape->surface().emittance() > 0.0) {
        colour += nearestShape->surface().emittance() * nearestShape->surface().colour();
    }

    //colour = nearestShape->surface().colour();
    //colour *= 12.0 / (nearestIntersectionDistance * nearestIntersectionDistance) * -(rn.direction() * nearestShape->calculateNormal(rn.origin() + rn.direction() * nearestIntersectionDistance));

    return colour;
}

#endif // Raytracer_HPP
