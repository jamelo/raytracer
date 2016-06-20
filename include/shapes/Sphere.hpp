#ifndef SHAPES_SPHERE_HPP
#define SHAPES_SPHERE_HPP

#include <shapes/Shape.hpp>

namespace shapes
{

    class Sphere : public Shape
    {
    private:
        Point3 m_origin;
        Vector3 m_up;
        double m_radius;
        double m_radiusSquared;

    public:
        Sphere(const Point3& _origin, const Vector3& _up, double _radius, const SurfaceDescription& _surface) :
        Shape(_surface),
        m_origin(_origin),
        m_up(_up),
        m_radius(_radius),
        m_radiusSquared(_radius * _radius)
        { }

        virtual IntersectionInfo calculateRayIntersection(const Ray3& ray) const {
            Vector3 newOrigin = m_origin - ray.origin();
            double projectedCentre = newOrigin * ray.direction();               //Project centre of sphere onto ray
            double discriminant = m_radiusSquared - (newOrigin * newOrigin - projectedCentre * projectedCentre);

            if (discriminant < 0) {
                return IntersectionInfo();
            }

            double squareRootDiscriminant = std::sqrt(discriminant);
            double p1 = projectedCentre - squareRootDiscriminant;
            double p2 = projectedCentre + squareRootDiscriminant;

            if (p1 > 0) {
                return IntersectionInfo(p1, this);
            } else if (p2 > 0) {
                return IntersectionInfo(p2, this);
            }

            return IntersectionInfo();
        }

        virtual Vector3 calculateNormal(const Point3& p) const {
            return normalize(p - m_origin);
        }

        virtual Point2 textureMap(const Point3&) const {
            //TODO: implement
            return Point2{0, 0};
        }
    };

}

#endif
