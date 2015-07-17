#ifndef Shape_HPP
#define Shape_HPP

#include <cmath>
#include <limits>

#include <Point.hpp>
#include <Ray.hpp>
#include <SurfaceDescription.hpp>
#include <Vector.hpp>

using namespace geometry;

class Shape
{
private:
    SurfaceDescription m_surface;

public:
    Shape(const SurfaceDescription& _surface) :
            m_surface(_surface) {
    }

    virtual double calculateRayIntersection(const Ray3& ray) const = 0;
    virtual Vector3 calculateNormal(const Point3& p) const = 0;

    const SurfaceDescription& surface() const { return m_surface; }
    SurfaceDescription& surface() { return m_surface; }
};

class Plane : public Shape
{
private:
    Point3 m_origin;
    Vector3 m_direction1;
    Vector3 m_direction2;
    Vector3 m_normal;

public:
    Plane(const Point3& _origin, const Vector3& _dir1, const Vector3& _dir2, const SurfaceDescription& _surface) :
        Shape(_surface),
        m_origin(_origin),
        m_direction1(normalize(_dir1)),
        m_direction2(normalize(_dir2)),
        m_normal(cross_product(_dir1, _dir2))
    {
        //TODO: validate inputs, throw exception
    }

    virtual double calculateRayIntersection(const Ray3& ray) const {
        Vector3 diff = m_origin - ray.origin();
        return (m_normal * (diff)) / (m_normal * ray.direction());
    }

    virtual Vector3 calculateNormal(__attribute__((unused)) const Point3& p) const {
        return m_normal;
    }
};

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

    virtual double calculateRayIntersection(const Ray3& ray) const {
        Vector3 newOrigin = m_origin - ray.origin();
        double projectedCentre = newOrigin * ray.direction();               //Project centre of sphere onto ray
        double discriminant = m_radiusSquared - (newOrigin * newOrigin - projectedCentre * projectedCentre);

        if (discriminant < 0) {
            return std::numeric_limits<double>::infinity();
        }

        double squareRootDiscriminant = std::sqrt(discriminant);
        double p1 = projectedCentre - squareRootDiscriminant;

        if (p1 > 0) {
            return p1;
        }

        double p2 = projectedCentre + squareRootDiscriminant;

        if (p2 > 0) {
            return p2;
        }

        return std::numeric_limits<double>::infinity();
    }

    virtual Vector3 calculateNormal(const Point3& p) const {
        return p - m_origin;
    }
};

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

    virtual double calculateRayIntersection(const Ray3& ray) const {
        Vector3 d = ray.direction();
        Vector3 o = ray.origin() - Point3{0, 0, 0};

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
            return std::numeric_limits<double>::infinity();
        }

        return t;
    }

    virtual Vector3 calculateNormal(const Point3& p) const {
        double dx = 4 * b * p.x() * p.x() * p.x() - 2 * c * p.x();
        double dy = 4 * b * p.y() * p.y() * p.y() - 2 * c * p.y();
        double dz = 4 * b * p.z() * p.z() * p.z() - 2 * c * p.z();

        return normalize(Vector3{dx, dy, dz});
    }
};

#endif // Shapes_HPP
