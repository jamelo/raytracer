#ifndef Shape_HPP
#define Shape_HPP

#include <cmath>
#include <limits>

#include <Point.hpp>
#include <Ray.hpp>
#include <SurfaceDescription.hpp>
#include <Transformation.hpp>
#include <Vector.hpp>

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
        m_normal(normalize(cross_product(_dir1, _dir2)))
    {
        //TODO: validate inputs, throw exception
    }

    virtual IntersectionInfo calculateRayIntersection(const Ray3& ray) const {
        Vector3 diff = m_origin - ray.origin();
        return IntersectionInfo((m_normal * (diff)) / (m_normal * ray.direction()), this);
    }

    virtual Vector3 calculateNormal(const Point3&) const {
        return m_normal;
    }

    virtual Point2 textureMap(const Point3& p) const {
        float dummy;
        float x = std::modf((p - m_origin) * m_direction1, &dummy);
        float y = std::modf((p - m_origin) * m_direction2, &dummy);

        x = (x >= 0 ? x : x + 1.0);
        y = (y >= 0 ? y : y + 1.0);

        return Point2{x, y};
    }
};

class Rectangle : public Shape
{
private:
    Point3 m_p0;
    Point3 m_p1;
    Point3 m_p2;
    Vector3 m_normal;
    Vector3 m_v0;
    Vector3 m_v1;
    float m_v0_v1;
    float m_v1_v1;
    float m_v0_v0;
    float m_recipDenominator;

public:
    Rectangle(const Point3& p0, const Point3& p1, const Point3& p2, const SurfaceDescription& surface) :
        Shape(surface),
        m_p0(p0),
        m_p1(p1),
        m_p2(p2),
        m_normal(normalize(cross_product(p1 - p0, p2 - p0))),
        m_v0(m_p2 - m_p0),
        m_v1(m_p1 - m_p0),
        m_v0_v1(m_v0 * m_v1),
        m_v1_v1(m_v1 * m_v1),
        m_v0_v0(m_v0 * m_v0),
        m_recipDenominator(1.0 / (m_v0_v0 * m_v1_v1 - m_v0_v1 * m_v0_v1))
    { }

    virtual IntersectionInfo calculateRayIntersection(const Ray3& ray) const {
        Vector3 diff = m_p0 - ray.origin();
        double distance = (m_normal * diff) / (m_normal * ray.direction());
        Point3 poi = ray.origin() + distance * ray.direction();

        Vector3 v2 = poi - m_p0;

        float u = (m_v1_v1 * (v2 * m_v0) - m_v0_v1 * (v2 * m_v1)) * m_recipDenominator;
        float v = (m_v0_v0 * (v2 * m_v1) - m_v0_v1 * (v2 * m_v0)) * m_recipDenominator;

        return (u >= 0 && v >= 0 && u <= 1 && v <= 1) ? IntersectionInfo(distance, this) : IntersectionInfo();
    }

    virtual Vector3 calculateNormal(const Point3&) const {
        return m_normal;
    }

    virtual Point2 textureMap(const Point3& p) const {
        float dummy;
        float x = std::modf((p - m_p0) * (m_p1 - m_p0), &dummy);
        float y = std::modf((p - m_p0) * (m_p2 - m_p0), &dummy);

        x = (x >= 0 ? x : x + 1.0);
        y = (y >= 0 ? y : y + 1.0);

        return Point2{x, y};
    }
};

class Box : public Shape
{
private:
    std::array<Rectangle, 6> m_sides;

public:
    Box(const Vector3& size, const Point3& location, const Vector3& orientation, const SurfaceDescription& surface) : Shape(surface),
    m_sides{Rectangle({0, 0, 0}, {0, 0, 0}, {0, 0, 0}, surface), Rectangle({0, 0, 0}, {0, 0, 0}, {0, 0, 0}, surface), Rectangle({0, 0, 0}, {0, 0, 0}, {0, 0, 0}, surface), Rectangle({0, 0, 0}, {0, 0, 0}, {0, 0, 0}, surface), Rectangle({0, 0, 0}, {0, 0, 0}, {0, 0, 0}, surface), Rectangle({0, 0, 0}, {0, 0, 0}, {0, 0, 0}, surface)}
    {
        Vector3 s = size * 0.5;
        Vector3 origin = static_cast<Vector<double, 3ul>>(location);

        auto rotationTransform = rotation(orientation[0], orientation[1], orientation[2]);

        std::array<Point3, 8> points{
            rotationTransform * Point3{-s[0],  s[1], -s[2]} + origin,
            rotationTransform * Point3{ s[0],  s[1], -s[2]} + origin,
            rotationTransform * Point3{ s[0],  s[1],  s[2]} + origin,
            rotationTransform * Point3{-s[0],  s[1],  s[2]} + origin,
            rotationTransform * Point3{-s[0], -s[1], -s[2]} + origin,
            rotationTransform * Point3{ s[0], -s[1], -s[2]} + origin,
            rotationTransform * Point3{ s[0], -s[1],  s[2]} + origin,
            rotationTransform * Point3{-s[0], -s[1],  s[2]} + origin
        };

        m_sides[0] = Rectangle(points[0], points[1], points[3], surface);
        m_sides[1] = Rectangle(points[4], points[5], points[0], surface);
        m_sides[2] = Rectangle(points[5], points[6], points[1], surface);
        m_sides[3] = Rectangle(points[6], points[7], points[2], surface);
        m_sides[4] = Rectangle(points[7], points[4], points[3], surface);
        m_sides[5] = Rectangle(points[7], points[4], points[6], surface);
    }

    virtual IntersectionInfo calculateRayIntersection(const Ray3& ray) const {
        IntersectionInfo nearestIntersection;

        for (const auto& side : m_sides) {
            IntersectionInfo i = side.calculateRayIntersection(ray);

            if (i.distance() < nearestIntersection.distance()) {
                nearestIntersection = i;
            }
        }

        return nearestIntersection;
    }

    virtual Vector3 calculateNormal(const Point3&) const {
        return Vector3{0, 0, 0};
    }

    virtual Point2 textureMap(const Point3& p) const {
        return Point2{0, 0};
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

#endif // Shapes_HPP
