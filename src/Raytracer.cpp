#include <Raytracer.hpp>

#include <cmath>
#include <random>
#include <vector>

#include <geometry/Ray.hpp>
#include <geometry/Vector.hpp>
#include <graphics/Colour.hpp>
#include <IntersectionInfo.hpp>
#include <RandomGenerator.hpp>
#include <Scene.hpp>

using namespace geometry;
using namespace graphics;
using namespace threading;

static constexpr double pi() { return std::atan(1.0) * 4.0; }

constexpr float epsilon = 1e-10;

ColourRgb<float> calculateRayColour(const Ray3& lightRay, const Scene& scene, int recursionDepth, double weight, const std::vector<float>& refractiveIndexStack = {1.0f}, const Vector3& lastNormal = {0, 0, 0});

Vector3 randomVectorOnUnitHemisphere(const Vector3& direction)
{
    thread_local std::uniform_real_distribution<float> dist(0, 1);

    Vector3 i = direction;
    Vector3 j, k;

    if (std::abs(i.x()) > std::abs(i.y()))
    {
        double recipLength = 1 / std::sqrt(i.x() * i.x() + i.z() * i.z());
        j = Vector3(-i.z() * recipLength, 0, i.x() * recipLength);
    }
    else
    {
        double recipLength = 1 / std::sqrt(i.y() * i.y() + i.z() * i.z());
        j = Vector3(0, -i.z() * recipLength, i.y() * recipLength);
    }

    k = cross_product(i, j);

    float theta = 2 * pi() * dist(RandomGenerator::get_instance());
    float phi = pi() * 0.5 - std::acos(std::acos(dist(RandomGenerator::get_instance())) * 2 / pi());

    return std::cos(phi) * i + std::cos(theta) * std::sin(phi) * j + std::sin(theta) * sin(phi) * k;
}

IntersectionInfo nearestShapeIntersection(const Ray3& ray, const Scene& scene)
{
    shapes::Shape::IntersectionResult nearest;

    for (auto&& shape : scene.geometry())
    {
        shapes::Shape::IntersectionResult intersection = shape->calculateRayIntersection(ray);

        if (intersection.distance() < nearest.distance() && intersection.distance() > epsilon)
        {
            nearest = intersection;
        }
    }

    return IntersectionInfo(ray, nearest);
}

ColourRgb<float> calculateDifuseReflection(const IntersectionInfo& info, const Ray3&, const Scene& scene, int recursionDepth, double weight, const std::vector<float>& refractiveIndexStack, const geometry::Vector3& lastNormal)
{
    Vector3 nextRayDirection = randomVectorOnUnitHemisphere(info.normal());
    Ray3 nextRay = Ray3(info.location(), nextRayDirection);
    double newWeight = info.surface().colour().average() * weight * (info.normal() * nextRay.direction());

    return calculateRayColour(nextRay, scene, recursionDepth + 1, newWeight, refractiveIndexStack, lastNormal) * info.surface().colour();
}

ColourRgb<float> calculateReflection(const IntersectionInfo& info, const Ray3& ray, const Scene& scene, int recursionDepth, double weight, const std::vector<float>& refractiveIndexStack)
{
    Vector3 nextRayDirection = ray.direction() - 2 * (ray.direction() * info.normal()) * info.normal();
    Ray3 nextRay = Ray3(info.location(), nextRayDirection);
    double newWeight = info.surface().colour().average() * weight;

    return calculateRayColour(nextRay, scene, recursionDepth + 1, newWeight, refractiveIndexStack) * info.surface().reflectance() * info.surface().colour();
}

ColourRgb<float> calculateTransmission(const IntersectionInfo& info, const Ray3& ray, const Scene& scene, int recursionDepth, double weight, const std::vector<float>& refractiveIndexStack)
{

    std::vector<float> newRefractiveIndexStack(refractiveIndexStack);

    float n1 = refractiveIndexStack.back();
    float n2 = 0.0;

    if (info.isEnteringSurface())
    {
        newRefractiveIndexStack.push_back(info.surface().refractiveIndex());
        n2 = newRefractiveIndexStack.back();
    }
    else
    {
        newRefractiveIndexStack.pop_back();
        n2 = newRefractiveIndexStack.back();
    }

    Vector3 rayProjectedOnNormal = (ray.direction() * info.normal()) * info.normal();

    //Snell's law
    Vector3 vsinTheta1 = -rayProjectedOnNormal + ray.direction();
    Vector3 vsinTheta2 = vsinTheta1 * (n1 / n2);
    Vector3 vcosTheta2 = -std::sqrt(1 - vsinTheta2 * vsinTheta2) * info.normal();

    Vector3 refractedRayDirection = vsinTheta2 + vcosTheta2;
    Vector3 reflectedRayDirection = ray.direction() - 2 * rayProjectedOnNormal;

    Ray3 refractedRay = Ray3(info.location(), refractedRayDirection);
    Ray3 reflectedRay = Ray3(info.location(), reflectedRayDirection);

    //Fresnel equations
    double cosTheta1 = -(ray.direction() * info.normal());
    double cosTheta2 = -(refractedRayDirection * info.normal());

    double rs = (n1 * cosTheta1 - n2 * cosTheta2) / (n1 * cosTheta1 + n2 * cosTheta2);
    double rp = (n2 * cosTheta1 - n1 * cosTheta2) / (n2 * cosTheta1 + n1 * cosTheta2);

    double fresnelReflectance = std::min((rs * rs + rp * rp) * 0.5, 1.0);
    fresnelReflectance = std::isnan(fresnelReflectance) ? 1.0 : fresnelReflectance;

    ColourRgb<float> refractedLight(0, 0, 0);
    ColourRgb<float> reflectedLight = calculateRayColour(reflectedRay, scene, recursionDepth + 1, weight, refractiveIndexStack);

    if (fresnelReflectance < 1.0)
    {
        double newWeight = info.surface().colour().average() * weight;
        refractedLight = calculateRayColour(refractedRay, scene, recursionDepth + 1, newWeight, newRefractiveIndexStack) * info.surface().colour();
    }

    return (reflectedLight * fresnelReflectance + refractedLight * (1.0 - fresnelReflectance)) * info.surface().transmittance();
}

bool clearLineOfSight(const Point3 p1, const Point3 p2, const Scene& scene)
{
    Vector3 direction = normalize(p2 - p1);
    Ray3 ray = Ray3(p1 + direction * epsilon, direction);

    IntersectionInfo info = nearestShapeIntersection(ray, scene);

    return info.distance() > (abs(p2 - p1) - epsilon * 2);
}

ColourRgb<float> calculateLightRay(const Ray3& ray, const Scene& scene, const geometry::Vector3& lastNormal)
{
    ColourRgb<float> lightColour(0, 0, 0);

    for (const auto& light : scene.lights())
    {
        Point3 lightPoint = light->sampleSurface();
        Vector3 direction = normalize(lightPoint - ray.origin());

        if (clearLineOfSight(ray.origin(), lightPoint, scene))
        {
            Vector3 zero(0.0, 0.0, 0.0);
            double cosineFactor = !(abs(lastNormal) < 0.0001) ? std::abs(direction * lastNormal) : 1.0;

            lightColour += light->surface().emittance() * light->surface().colour() * cosineFactor;
        }
    }

    return lightColour;
}



//TODO: add case for transmittance
//TODO: break this function into smaller functions: one function for each surface type. sum results
ColourRgb<float> calculateRayColour(const Ray3& ray, const Scene& scene, int recursionDepth = 0, double weight = 1.0, const std::vector<float>& refractiveIndexStack, const geometry::Vector3& lastNormal)
{
    //Ray3 ray = Ray3(lightRay.origin(), normalize(lightRay.direction()));
    double survivalProb = (recursionDepth < 2) ? 1.0 : 0.8;
    int recursionLimit = 20;

    // Return black if recursion limit hit
    if (recursionDepth >= recursionLimit || weight < (1.0 / 255.0))
    {
        return calculateLightRay(ray, scene, lastNormal);
    }

    // Russian roulette
    {
        thread_local std::uniform_real_distribution<float> dist(0, 1);

        if (dist(RandomGenerator::get_instance()) > survivalProb)
        {
            return calculateLightRay(ray, scene, lastNormal);
        }
    }

    IntersectionInfo info = nearestShapeIntersection(ray, scene);

    // Return black if ray did not intersect with any geometry
    if (!info)
    {
        return ColourRgb<float>(0, 0, 0);
    }

    // Return black if ray is parallel to surface
    if (info.cosAngleOfIncidence() < epsilon)
    {
        return ColourRgb<float>(0, 0, 0);
    }

    ColourRgb<float> colour{0, 0, 0};

    if (info.surface().difuseReflectance() > 0.0)
    {
        colour += calculateDifuseReflection(info, ray, scene, recursionDepth, weight, refractiveIndexStack, info.normal());
    }

    if (info.surface().reflectance() > 0.0)
    {
        colour += calculateReflection(info, ray, scene, recursionDepth, weight, refractiveIndexStack);
    }

    if (info.surface().transmittance() > 0.0)
    {
        colour += calculateTransmission(info, ray, scene, recursionDepth, weight, refractiveIndexStack);
    }

    if (info.surface().emittance() > 0.0)
    {
        colour += info.surface().emittance() * info.surface().colour();
    }

    return colour * (1.0 / survivalProb);
}

TaskHandle render(ThreadPool& pool, const std::shared_ptr<Scene>& scene)
{
    return scene->camera().render(pool, [=](const Ray3& ray) { return calculateRayColour(ray, *scene); });
}
