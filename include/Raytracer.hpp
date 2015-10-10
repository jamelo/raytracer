#ifndef Raytracer_HPP
#define Raytracer_HPP

#include <random>
#include <vector>

#include <Camera.hpp>
#include <LightSource.hpp>
#include <Shape.hpp>

class Scene;

constexpr float epsilon = 1e-10;

ColourRgb<float> calculateRayColour(const Ray3& r, const Scene& scene, int recursion_depth, std::vector<float> refractiveIndexStack = {1.0f});

class Scene
{
private:
    Camera m_camera;
    std::vector<std::shared_ptr<Shape>> m_shapes;

public:
    Scene(const Camera& _cam, std::vector<std::shared_ptr<Shape>>&& _shapes) :
            m_camera(_cam),
            m_shapes(std::move(_shapes)) {
    }

    Scene(const Camera& _cam, const std::vector<std::shared_ptr<Shape>>& _shapes) :
        m_camera(_cam),
        m_shapes(_shapes) {
    }

    const Camera& camera() const {
        return m_camera;
    }

    const std::vector<std::shared_ptr<Shape>>& shapes() const {
        return m_shapes;
    }

    //TODO: find a more appropriate place to pass in an anti aliaser. Should it not be a property of the camera instead of a property of the scene?
    TaskHandle render(ThreadPool& pool) const {
        Scene s{*this};
        return m_camera.render(pool, [=](const Ray3& r) { return calculateRayColour(r, s, 7); });
    }
};

//TODO: find a more appropriate place to define pi
constexpr double pi() { return std::atan(1.0) * 4.0; }

inline Vector3 randomVectorOnUnitHemisphere(const Vector3& direction)
{
    thread_local std::uniform_real_distribution<float> dist(0, 1);

    Vector3 nonCollinear = std::abs(direction * Vector3{1, 0, 0}) < 0.8 ? Vector3{1, 0, 0} : Vector3{0, 1, 0};
    Vector3 i = direction;
    Vector3 j = normalize(cross_product(direction, nonCollinear));
    Vector3 k = normalize(cross_product(i, j));

    float theta = 2 * pi() * dist(RandomGenerator::get_instance());
    float phi = pi() * 0.5 - std::acos(std::acos(dist(RandomGenerator::get_instance())) * 2 / pi());

    return std::cos(phi) * i + std::cos(theta) * std::sin(phi) * j + std::sin(theta) * sin(phi) * k;
}

//TODO: add case for transmittance
//TODO: break this function into smaller functions: one function for each surface type. sum results
ColourRgb<float> calculateRayColour(const Ray3& r, const Scene& scene, int recursion_depth, std::vector<float> refractiveIndexStack)
{
    if (recursion_depth == 0) {
        return ColourRgb<float>(0, 0, 0);
    }

    IntersectionInfo nearestIntersection;
    ColourRgb<float> colour{0, 0, 0};
    Ray3 rn = Ray3(r.origin(), normalize(r.direction()));

    for (auto&& shape : scene.shapes()) {
        IntersectionInfo intersection = shape->calculateRayIntersection(rn);

        if (intersection.distance() < nearestIntersection.distance() && intersection.distance() > epsilon) {
            nearestIntersection = intersection;
        }
    }

    if (!nearestIntersection.shape()) {
        return ColourRgb<float>(0, 0, 0);
    }

    const Shape& nearestShape = *(nearestIntersection.shape());
    const SurfaceDescription& surface = nearestShape.surface();
    Point3 pointOfIntersection = rn.origin() + rn.direction() * nearestIntersection.distance();
    Vector3 surfaceNormal = nearestShape.calculateNormal(pointOfIntersection);
    bool enteringSurface = true;
    double cosAngleOfIncidence = std::abs(surfaceNormal * rn.direction());

    if (cosAngleOfIncidence < epsilon) {
        return ColourRgb<float>(0, 0, 0);
    }

    if (surfaceNormal * rn.direction() > 0.0) {
        surfaceNormal = -surfaceNormal;
        enteringSurface = false;
    }

    if (surface.difuseReflectance() > 0.0) {
        Vector3 nextRayDirection = randomVectorOnUnitHemisphere(surfaceNormal);
        Ray3 nextRay = Ray3(pointOfIntersection, nextRayDirection);

        colour += calculateRayColour(nextRay, scene, recursion_depth - 1, refractiveIndexStack) * surface.colour();
    }

    if (surface.reflectance() > 0.0) {
        Vector3 nextRayDirection = rn.direction() - 2 * (rn.direction() * surfaceNormal) * surfaceNormal;
        Ray3 nextRay = Ray3(pointOfIntersection, nextRayDirection);

        colour += calculateRayColour(nextRay, scene, recursion_depth - 1, refractiveIndexStack) * surface.reflectance() * surface.colour();
    }

    if (surface.transmittance() > 0.0) {
        std::vector<float> newRefractiveIndexStack(refractiveIndexStack);

        float n1 = refractiveIndexStack.back();
        float n2 = 0.0;

        if (enteringSurface) {
            newRefractiveIndexStack.push_back(surface.refractiveIndex());
            n2 = newRefractiveIndexStack.back();
        } else {
            newRefractiveIndexStack.pop_back();
            n2 = newRefractiveIndexStack.back();
        }

        //Snell's law
        Vector3 vsinTheta1 = -(rn.direction() * surfaceNormal) * surfaceNormal + rn.direction();
        Vector3 vsinTheta2 = vsinTheta1 * (n1 / n2);
        Vector3 vcosTheta2 = -std::sqrt(1 - vsinTheta2 * vsinTheta2) * surfaceNormal;

        Vector3 refractedRayDirection = vsinTheta2 + vcosTheta2;
        Vector3 reflectedRayDirection = rn.direction() - 2 * (rn.direction() * surfaceNormal) * surfaceNormal;
        Ray3 refractedRay = Ray3(pointOfIntersection, refractedRayDirection);
        Ray3 reflectedRay = Ray3(pointOfIntersection, reflectedRayDirection);

        //Fresnel equations
        double cosTheta1 = -(rn.direction() * surfaceNormal);
        double cosTheta2 = -(refractedRayDirection * surfaceNormal);
        double rs = (n1 * cosTheta1 - n2 * cosTheta2) / (n1 * cosTheta1 + n2 * cosTheta2);
        double rp = (n2 * cosTheta1 - n1 * cosTheta2) / (n2 * cosTheta1 + n1 * cosTheta2);
        double fresnelReflectance = std::min((rs * rs + rp * rp) * 0.5, 1.0);
        fresnelReflectance = std::isnan(fresnelReflectance) ? 1.0 : fresnelReflectance;


        auto refractedLight = (fresnelReflectance < 1.0) ? calculateRayColour(refractedRay, scene, recursion_depth - 1, newRefractiveIndexStack) * surface.colour() : ColourRgb<float>(0, 0, 0);
        auto reflectedLight = calculateRayColour(reflectedRay, scene, recursion_depth - 1, refractiveIndexStack);

        colour += (reflectedLight * fresnelReflectance + refractedLight * (1.0 - fresnelReflectance)) * surface.transmittance();
    }

    if (surface.emittance() > 0.0) {
        colour += surface.emittance() * surface.colour();
    }

    return colour;
}

#endif // Raytracer_HPP
