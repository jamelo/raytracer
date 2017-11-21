#ifndef SHAPES_PLANE_HPP
#define SHAPES_PLANE_HPP

#include <Exceptions.hpp>
#include <shapes/Shape.hpp>
#include <builders/CustomShapeBuilder.hpp>

namespace shapes
{

    class Plane : public Shape
    {
    private:
        geometry::Point3 m_origin;
        geometry::Vector3 m_direction1;
        geometry::Vector3 m_direction2;
        geometry::Vector3 m_normal;

    public:
        Plane(const geometry::Point3& _origin, const geometry::Vector3& _dir1, const geometry::Vector3& _dir2, const std::shared_ptr<Surface>& _surface) :
            Shape(_surface),
            m_origin(_origin),
            m_direction1(geometry::normalize(_dir1)),
            m_direction2(geometry::normalize(_dir2)),
            m_normal(geometry::normalize(geometry::cross_product(_dir1, _dir2)))
        {
            //TODO: validate inputs, throw exception
        }

        Plane(const geometry::Point3& origin, const geometry::Vector3& normal, const std::shared_ptr<Surface>& surface) :
            Shape(surface),
            m_origin(origin),
            m_normal(geometry::normalize(normal))
        {
            if (std::abs(m_normal.x()) > 0.1 || std::abs(m_normal.y()) > 0.1)
            {
                m_direction1 = geometry::normalize(Vector3(m_normal.y(), -m_normal.x(), 0));
            }
            else if (std::abs(m_normal.x()) > 0.1 || std::abs(m_normal.z()) > 0.1)
            {
                m_direction1 = geometry::normalize(Vector3(m_normal.z(), 0, -m_normal.z()));
            }
            else
            {
                throw SomeKindOfException();
            }

            m_direction2 = geometry::cross_product(m_direction1, m_normal);
        }

        virtual IntersectionResult calculateRayIntersection(const geometry::Ray3& ray) const
        {
            geometry::Vector3 diff = m_origin - ray.origin();
            return IntersectionResult((m_normal * (diff)) / (m_normal * ray.direction()), this);
        }

        virtual geometry::Vector3 calculateNormal(const geometry::Point3&) const
        {
            return m_normal;
        }

        virtual geometry::Point2 textureMap(const geometry::Point3& p) const
        {
            float dummy;
            float x = std::modf((p - m_origin) * m_direction1, &dummy);
            float y = std::modf((p - m_origin) * m_direction2, &dummy);

            x = (x >= 0 ? x : x + 1.0);
            y = (y >= 0 ? y : y + 1.0);

            return geometry::Point2{x, y};
        }
    };

    class PlaneBuilder : public builders::CustomShapeBuilder
    {
    public:
        PlaneBuilder()
        {
            using namespace builders;

            parameter("location", ParamType::ePoint3, REQUIRED);
            parameter("normal", ParamType::eVector3, REQUIRED);
            parameter("surface", ParamType::eSurface, REQUIRED);
        }

    private:
        static builders::ShapeBuilder::Registration sm_registration;

        virtual std::shared_ptr<Shape> construct(const builders::BuilderArgs& args)
        {
            Point3 location = args.get<Point3>("location");
            Vector3 normal = args.get<Vector3>("normal");
            const auto& surface = args.get<std::shared_ptr<Surface>>("surface");

            return std::make_shared<Plane>(location, normal, surface);
        }
    };

    builders::ShapeBuilder::Registration PlaneBuilder::sm_registration("plane", std::make_unique<PlaneBuilder>());

}

#endif
