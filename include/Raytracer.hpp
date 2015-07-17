#ifndef Raytracer_HPP
#define Raytracer_HPP

#include <random>
#include <vector>

#include <Camera.hpp>
#include <LightSource.hpp>
#include <Shape.hpp>

class Scene;

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

    Image<ColourRgb<float>> render() const {
        return m_camera.render([&](const Ray3& r) { return calculateRayColour(r, *this, 5); }, AntiAliaserRandom<16>());
    }
};

constexpr double pi() { return std::atan(1.0) * 4.0; }

inline Vector3 randomVectorOnUnitSphere()
{
    thread_local std::mt19937 randgen;
    thread_local std::uniform_real_distribution<double> dist(0, 1);

    double theta = 2 * pi() * dist(randgen);
    double phi = std::acos(2 * dist(randgen) - 1);

    return Vector3{cos(theta), sin(theta), cos(phi)};
}

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

        if (intersectionDistance < nearestIntersectionDistance && intersectionDistance > 0) {
            nearestIntersectionDistance = intersectionDistance;
            nearestShape = shape.get();
        }
    }

    if (!nearestShape) {
        return ColourRgb<float>(0, 0, 0);
    }

    Point3 pointOfIntersection = rn.origin() + rn.direction() * nearestIntersectionDistance;
    Vector3 surfaceNormal = nearestShape->calculateNormal(pointOfIntersection);

    if (nearestShape->surface().difuseReflectance() > 0.0) {
        Vector3 nextRayDirection = randomVectorOnUnitSphere();

        if (nextRayDirection * surfaceNormal < 0.0) {
            nextRayDirection = -nextRayDirection;
        }

        Ray3 nextRay = Ray3(pointOfIntersection, nextRayDirection);
        double lambertianFactor = nextRayDirection * surfaceNormal;

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
