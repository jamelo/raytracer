#ifndef PATH_HPP
#define PATH_HPP

#include <vector>

#include <geometry/Point.hpp>
#include <geometry/Ray.hpp>
#include <geometry/Vector.hpp>
#include <graphics/Colour.hpp>

class PathPoint
{
public:
    geometry::Point3 location() const
    {
        return m_location;
    }

    geometry::Vector3 normal() const
    {
        retun m_normal;
    }

    graphics::ColourRgb<float> colour() const
    {
        return m_colour;
    }

private:
    geometry::Point3 m_location;
    geometry::Vector3 m_normal;
    graphics::ColourRgb<float> m_colour;
};

class Path
{
public:
    size_t size() const
    {
        return m_points.size();
    }

    const PathPoint& point(size_t number) const
    {
        return m_points[number];
    }

    void addPoint(const PathPoint& point)
    {
        m_points.push_back(point);
    }

private:
    std::vector<PathPoint> m_points;
};

#endif
