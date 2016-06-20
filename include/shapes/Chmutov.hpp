#ifndef SHAPES_CHMUTOV_HPP
#define SHAPES_CHMUTOV_HPP

#include <shapes/Shape.hpp>

namespace shapes
{

    class Chmutov : public Shape
    {
    private:
        Point3 m_origin;
        Vector3 m_up;
        double m_outerRadius;
        double m_innerRadius;

        double a = 3;
        double b = 8;
        double c = 8;

        double epsilon = 1e-10;

    public:
        Chmutov(const Point3& _origin, const Vector3& _up, double _outerRadius, double _innerRadius, const SurfaceDescription& _surface) :
        Shape(_surface),
        m_origin(_origin),
        m_up(_up),
        m_outerRadius(_outerRadius),
        m_innerRadius(_innerRadius)
        { }

        virtual IntersectionInfo calculateRayIntersection(const Ray3& ray) const {
            Vector3 d = ray.direction();
            Vector3 o = ray.origin() - Point3{1, 1, 0};

            Vector3 d2 = {d.x() * d.x(), d.y() * d.y(), d.z() * d.z()};
            Vector3 o2 = {o.x() * o.x(), o.y() * o.y(), o.z() * o.z()};
            Vector3 od = {o.x() * d.x(), o.y() * d.y(), o.z() * d.z()};

            double A = b * (d2 * d2);
            double B = 4 * b * (od * d2);
            double C = 6 * b * (o2 * d2) - c * d * d;
            double D = 4 * b * (o2 * od) - 2 * c * o * d;
            double E = a + b * (o2 * o2) - c * o * o;

            double t0 = 0;
            double t = 0;

            do {
                t0 = t;

                double t02 = t0 * t0;
                double f = A * t02 * t02 + B * t02 * t0 + C * t02 + D * t0 + E;
                //double f_prime = 4 * A * t02 * t0 + 3 * B * t02 + 2 * C * t0 + D;

                t = t0 + f * 0.0005; //f_prime;
            } while (std::abs(t - t0) / t > 0.0000001 && t < 20);

            if (t > 20) {
                return IntersectionInfo();
            }

            return IntersectionInfo(t, this);
        }

        virtual Vector3 calculateNormal(const Point3& p) const {
            float dx = 4 * b * p.x() * p.x() * p.x() - 2 * c * p.x();
            float dy = 4 * b * p.y() * p.y() * p.y() - 2 * c * p.y();
            float dz = 4 * b * p.z() * p.z() * p.z() - 2 * c * p.z();

            return normalize(Vector3{dx, dy, dz});
        }

        virtual Point2 textureMap(const Point3&) const {
            //TODO: implement
            return Point2{0, 0};
        }
    };

}

#endif
