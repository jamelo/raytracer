#ifndef INTERSECTION_INFO_HPP
#define INTERSECTION_INFO_HPP

#include <cassert>

#include <geometry/Point.hpp>
#include <geometry/Vector.hpp>
#include <shapes/Shape.hpp>
#include <Surface.hpp>

class IntersectionInfo
{
public:
    IntersectionInfo(const geometry::Ray3& ray, const shapes::Shape::IntersectionResult& result) :
        m_shape(result.shape()),
        m_distance(result.distance())
    {
        if (m_shape)
        {
            calculate(ray);
        }
    }

    operator bool()
    {
        return bool(m_shape);
    }

    bool isEmpty() const
    {
        return !m_shape;
    }

    bool isEnteringSurface() const
    {
        return m_enteringSurface;
    }

    double distance() const
    {
        return m_distance;
    }

    const geometry::Point3& location() const
    {
        return m_location;
    }

    const geometry::Vector3& normal() const
    {
        return m_normal;
    }

    const Surface& surface() const
    {
        assert(m_shape);
        return m_shape->surface();
    }

    double cosAngleOfIncidence() const
    {
        return m_cosAngleOfIncidence;
    }

private:
    void calculate(const geometry::Ray3& ray)
    {
        m_location = ray.origin() + ray.direction() * m_distance;
        m_normal = m_shape->calculateNormal(m_location);
        m_cosAngleOfIncidence = m_normal * ray.direction();
        m_enteringSurface = m_cosAngleOfIncidence < 0.0;

        if (m_enteringSurface)
        {
            // Ensure angle of incidence is positive
            m_cosAngleOfIncidence = -m_cosAngleOfIncidence;
        }
        else
        {
            // Ensure normal points away from surface
            m_normal = -m_normal;
        }
    }

    const shapes::Shape* m_shape;
    double m_distance;
    geometry::Point3 m_location;
    geometry::Vector3 m_normal;
    double m_cosAngleOfIncidence;
    bool m_enteringSurface;
};

#endif
