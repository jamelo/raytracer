#ifndef SHAPES_CHMUTOV_HPP
#define SHAPES_CHMUTOV_HPP

#include <builders/CustomShapeBuilder.hpp>
#include <shapes/Shape.hpp>

namespace shapes
{

    class Chmutov : public Shape
    {

    public:
        Chmutov(const geometry::Point3& origin, const geometry::Vector3& up, double outerRadius, double innerRadius, const std::shared_ptr<Surface>& surface) :
            Shape(surface),
            m_origin(origin),
            m_up(up),
            m_outerRadius(outerRadius),
            m_innerRadius(innerRadius),
            m_boundingBox(Vector3(4, 4, 4), origin, Vector3(0, 0, 0), surface)
        {

        }

        virtual IntersectionResult calculateRayIntersection(const geometry::Ray3& ray) const
        {
            geometry::Vector3 d = ray.direction();
            geometry::Vector3 o = ray.origin() - m_origin;

            IntersectionResult i = m_boundingBox.calculateRayIntersection(ray);

            if (std::isinf(i.distance()))
            {
                return IntersectionResult();
            }

            geometry::Vector3 d2 = {d.x() * d.x(), d.y() * d.y(), d.z() * d.z()};
            geometry::Vector3 o2 = {o.x() * o.x(), o.y() * o.y(), o.z() * o.z()};
            geometry::Vector3 od = {o.x() * d.x(), o.y() * d.y(), o.z() * d.z()};

            double A = b * (d2 * d2);
            double B = 4 * b * (od * d2);
            double C = 6 * b * (o2 * d2) - c * d * d;
            double D = 4 * b * (o2 * od) - 2 * c * o * d;
            double E = a + b * (o2 * o2) - c * o * o;

            double t0 = 0;
            double t = i.distance();

            do
            {
                t0 = t;

                double t02 = t0 * t0;
                double f = A * t02 * t02 + B * t02 * t0 + C * t02 + D * t0 + E;
                //double f_prime = 4 * A * t02 * t0 + 3 * B * t02 + 2 * C * t0 + D;

                double af = std::abs(f);

                if (af < 10)
                {
                    t = t0 + f * 0.003;
                }
                else
                {
                    t = t0 + f * 0.0003;
                }

                if (t > 20) {
                    return IntersectionResult();
                }
            } while (std::abs(t - t0) / t > 0.0000001);


            return IntersectionResult(t, this);
        }

        virtual geometry::Vector3 calculateNormal(const geometry::Point3& p) const
        {
            float dx = 4 * b * p.x() * p.x() * p.x() - 2 * c * p.x();
            float dy = 4 * b * p.y() * p.y() * p.y() - 2 * c * p.y();
            float dz = 4 * b * p.z() * p.z() * p.z() - 2 * c * p.z();

            return geometry::normalize(geometry::Vector3{dx, dy, dz});
        }

        virtual geometry::Point2 textureMap(const geometry::Point3&) const
        {
            //TODO: implement
            return geometry::Point2{0, 0};
        }

    private:
        geometry::Point3 m_origin;
        geometry::Vector3 m_up;
        double m_outerRadius;
        double m_innerRadius;
        Box m_boundingBox;

        double a = 3;
        double b = 2;
        double c = 4.2;

        double epsilon = 1e-10;
    };

    class ChmutovBuilder : public builders::CustomShapeBuilder
    {
    public:
        ChmutovBuilder() {
            using namespace builders;

            parameter("location", ParamType::ePoint3, REQUIRED);
            parameter("up", ParamType::eVector3, OPTIONAL, Vector3(0, 0, 0));
            parameter("outer-radius", ParamType::eFloat, OPTIONAL, 0.0);
            parameter("inner-radius", ParamType::eFloat, OPTIONAL, 0.0);
            parameter("surface", ParamType::eSurface, REQUIRED);
        }

    private:
        static builders::ShapeBuilder::Registration sm_registration;

        virtual std::shared_ptr<Shape> construct(const builders::BuilderArgs& args) {
            Point3 location = args.get<Point3>("location");
            Vector3 up = args.get<Vector3>("up");
            double outerRadius = args.get<double>("outer-radius");
            double innerRadius = args.get<double>("inner-radius");
            const auto& surface = args.get<std::shared_ptr<Surface>>("surface");

            return std::make_shared<Chmutov>(location, up, outerRadius, innerRadius, surface);
        }
    };

    builders::ShapeBuilder::Registration ChmutovBuilder::sm_registration("chmutov", std::make_unique<ChmutovBuilder>());

}

#endif
