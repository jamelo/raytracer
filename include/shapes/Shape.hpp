#ifndef SHAPE_HPP
#define SHAPE_HPP

#include <cmath>
#include <limits>

#include <geometry/Point.hpp>
#include <geometry/Ray.hpp>
#include <geometry/Transformation.hpp>
#include <geometry/Vector.hpp>
#include <Surface.hpp>

namespace shapes
{

    class Shape
    {
    public:
        class IntersectionResult
        {
        private:
            double m_distance;
            const shapes::Shape* m_shape;

        public:
            IntersectionResult(double distance = std::numeric_limits<double>::infinity(), const shapes::Shape* shape = nullptr) :
                m_distance(distance),
                m_shape(shape)
            { }

            double distance() const { return m_distance; }
            const shapes::Shape* shape() const { return m_shape; }
        };

        Shape(const std::shared_ptr<Surface>& surface) :
            m_surface(surface) {
        }

        virtual ~Shape()
        {

        }

        virtual IntersectionResult calculateRayIntersection(const geometry::Ray3& ray) const = 0;

        virtual geometry::Vector3 calculateNormal(const geometry::Point3& p) const = 0;

        virtual geometry::Point2 textureMap(const geometry::Point3& p) const = 0;

        virtual double surfaceArea() const
        {
            throw -1;
        }

        virtual geometry::Point3 sampleSurface() const
        {
            throw -1;
        }

        const Surface& surface() const { return *m_surface; }

    private:
        std::shared_ptr<Surface> m_surface;
    };

}

#endif
