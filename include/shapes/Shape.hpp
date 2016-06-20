#ifndef SHAPE_HPP
#define SHAPE_HPP

#include <cmath>
#include <limits>

#include <geometry/Point.hpp>
#include <geometry/Ray.hpp>
#include <geometry/Transformation.hpp>
#include <geometry/Vector.hpp>
#include <SurfaceDescription.hpp>

using namespace geometry;

class Shape;

class IntersectionInfo
{
private:
    double m_distance;
    const Shape* m_shape;

public:
    IntersectionInfo(double _distance = std::numeric_limits<double>::infinity(), const Shape* _shape = nullptr) :
        m_distance(_distance),
        m_shape(_shape)
    { }

    double distance() const { return m_distance; }
    const Shape* shape() const { return m_shape; }
};

class Shape
{
private:
    SurfaceDescription m_surface;

public:
    Shape(const SurfaceDescription& _surface) :
            m_surface(_surface) {
    }

    virtual IntersectionInfo calculateRayIntersection(const Ray3& ray) const = 0;
    virtual Vector3 calculateNormal(const Point3& p) const = 0;
    virtual Point2 textureMap(const Point3& p) const = 0;

    const SurfaceDescription& surface() const { return m_surface; }
    SurfaceDescription& surface() { return m_surface; }
};

#endif // Shapes_HPP
