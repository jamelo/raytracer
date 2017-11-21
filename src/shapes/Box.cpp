#include <memory>

#include <shapes/Shape.hpp>
#include <builders/BuilderBase.hpp>

class Box : public shapes::Shape
{
private:
    std::array<Rectangle, 6> m_sides;

public:

    Box(const geometry::Vector3& size, const geometry::Point3& location, const geometry::Vector3& orientation, const SurfaceDescription& surface) :
    Shape(surface),
    m_sides{
        Rectangle({0, 0, 0}, {0, 0, 0}, {0, 0, 0}, surface),
        Rectangle({0, 0, 0}, {0, 0, 0}, {0, 0, 0}, surface),
        Rectangle({0, 0, 0}, {0, 0, 0}, {0, 0, 0}, surface),
        Rectangle({0, 0, 0}, {0, 0, 0}, {0, 0, 0}, surface),
        Rectangle({0, 0, 0}, {0, 0, 0}, {0, 0, 0}, surface),
        Rectangle({0, 0, 0}, {0, 0, 0}, {0, 0, 0}, surface)
    }
    {
        geometry::Vector3 s = size * 0.5;
        geometry::Vector3 origin = static_cast<geometry::Vector<double, 3ul>>(location);

        auto rotationTransform = geometry::rotation(orientation[0], orientation[1], orientation[2]);

        std::array<geometry::Point3, 8> points{
            rotationTransform * geometry::Point3{-s[0],  s[1], -s[2]} + origin,
            rotationTransform * geometry::Point3{ s[0],  s[1], -s[2]} + origin,
            rotationTransform * geometry::Point3{ s[0],  s[1],  s[2]} + origin,
            rotationTransform * geometry::Point3{-s[0],  s[1],  s[2]} + origin,
            rotationTransform * geometry::Point3{-s[0], -s[1], -s[2]} + origin,
            rotationTransform * geometry::Point3{ s[0], -s[1], -s[2]} + origin,
            rotationTransform * geometry::Point3{ s[0], -s[1],  s[2]} + origin,
            rotationTransform * geometry::Point3{-s[0], -s[1],  s[2]} + origin
        };

        m_sides[0] = Rectangle(points[0], points[1], points[3], surface);
        m_sides[1] = Rectangle(points[4], points[5], points[0], surface);
        m_sides[2] = Rectangle(points[5], points[6], points[1], surface);
        m_sides[3] = Rectangle(points[6], points[7], points[2], surface);
        m_sides[4] = Rectangle(points[7], points[4], points[3], surface);
        m_sides[5] = Rectangle(points[7], points[4], points[6], surface);
    }

    virtual IntersectionInfo calculateRayIntersection(const geometry::Ray3& ray) const {
        IntersectionInfo nearestIntersection;

        for (const auto& side : m_sides) {
            IntersectionInfo i = side.calculateRayIntersection(ray);

            if (i.distance() < nearestIntersection.distance()) {
                nearestIntersection = i;
            }
        }

        return nearestIntersection;
    }

    virtual geometry::Vector3 calculateNormal(const geometry::Point3&) const {
        return geometry::Vector3{0, 0, 0};
    }

    virtual geometry::Point2 textureMap(const geometry::Point3& p) const {
        return geometry::Point2{0, 0};
    }
};


class BoxBuilder : public builders::BuilderBase<Shape>
{
    using namespace builders;

public:
    BoxBuilder()
    {
        parameter("size", ParamType::eVector3, REQUIRED);
        parameter("location", ParamType::ePoint3, REQUIRED);
        parameter("orientation", ParamType::eVector3, OPTIONAL, geometry::Point3(0, 0, 0));
        parameter("surface", ParamType::eSurface, REQUIRED);
    }

private:
    static ShapeBuilder::Registration sm_registration;

    virtual std::unique_ptr<Shape> construct(const BuilderArgs& args)
    {
        const geometry::Vector3 size = args.get<geometry::Vector3>("size");
        const geometry::Point3 location = args.get<geometry::Point3>("location");
        const geometry::Vector3 orientation = args.get<geometry::Vector3>("orientation");
        const SurfaceDescription& surface = args.get<SurfaceDescription>("surface");

        return std::make_unique<Box>(size, location, orientation, surface);
    }
};

ShapeBuilder::Registration BoxBuilder::sm_registration(std::make_unique<BoxBuilder>());

#endif
