#ifndef SHAPES_SPHERE_HPP
#define SHAPES_SPHERE_HPP

#include <builders/CustomShapeBuilder.hpp>
#include <shapes/Shape.hpp>

namespace shapes
{

    class Sphere : public Shape
    {
    private:
        geometry::Point3 m_origin;
        geometry::Vector3 m_up;
        double m_radius;
        double m_radiusSquared;

    public:
        Sphere(const geometry::Point3& _origin, const geometry::Vector3& _up, double _radius, const std::shared_ptr<Surface>& _surface) :
            Shape(_surface),
            m_origin(_origin),
            m_up(_up),
            m_radius(_radius),
            m_radiusSquared(_radius * _radius)
        {

        }

        virtual IntersectionResult calculateRayIntersection(const geometry::Ray3& ray) const
        {
            geometry::Vector3 newOrigin = m_origin - ray.origin();
            double projectedCentre = newOrigin * ray.direction();               //Project centre of sphere onto ray
            double discriminant = m_radiusSquared - (newOrigin * newOrigin - projectedCentre * projectedCentre);

            if (discriminant < 0) {
                return IntersectionResult();
            }

            double squareRootDiscriminant = std::sqrt(discriminant);
            double p1 = projectedCentre - squareRootDiscriminant;
            double p2 = projectedCentre + squareRootDiscriminant;

            if (p1 > 0)
            {
                return IntersectionResult(p1, this);
            }
            else if (p2 > 0)
            {
                return IntersectionResult(p2, this);
            }

            return IntersectionResult();
        }

        virtual geometry::Vector3 calculateNormal(const geometry::Point3& p) const
        {
            return geometry::normalize(p - m_origin);
        }

        virtual geometry::Point2 textureMap(const geometry::Point3&) const
        {
            //TODO: implement
            return geometry::Point2{0, 0};
        }
    };

    class SphereBuilder : public builders::CustomShapeBuilder
    {
    public:
        SphereBuilder()
        {
            using namespace builders;

            parameter("location", ParamType::ePoint3, REQUIRED);
            parameter("radius", ParamType::eFloat, REQUIRED);
            parameter("orientation", ParamType::eVector3, OPTIONAL, Vector3(0, 0, 0));
            parameter("surface", ParamType::eSurface, REQUIRED);
        }

    private:
        static builders::ShapeBuilder::Registration sm_registration;

        virtual std::shared_ptr<Shape> construct(const builders::BuilderArgs& args)
        {
            Point3 location = args.get<Point3>("location");
            double radius = args.get<double>("radius");
            Vector3 orientation = args.get<Vector3>("orientation");
            const auto& surface = args.get<std::shared_ptr<Surface>>("surface");

            return std::make_shared<Sphere>(location, orientation, radius, surface);
        }
    };

    builders::ShapeBuilder::Registration SphereBuilder::sm_registration("sphere", std::make_unique<SphereBuilder>());

}

#endif
