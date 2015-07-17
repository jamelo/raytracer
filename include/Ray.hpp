#ifndef Ray_HPP
#define Ray_HPP

#include <Point.hpp>
#include <Vector.hpp>

namespace geometry
{

    template <typename T, size_t Dimensions>
    class Ray
    {
    private:
        Point<T, Dimensions> m_origin;
        Vector<T, Dimensions> m_direction;

    public:
        Ray(const Point<T, Dimensions>& _origin, const Vector<T, Dimensions>& _direction) :
                m_origin(_origin),
                m_direction(_direction) {
        }

        const Point<T, Dimensions>& origin() const { return m_origin; }
        const Vector<T, Dimensions>& direction() const { return m_direction; }
        Point<T, Dimensions>& origin() { return m_origin; }
        Vector<T, Dimensions>& direction() { return m_direction; }
    };

    typedef Ray<geo_type, 2> Ray2;
    typedef Ray<geo_type, 3> Ray3;

}

#endif // Ray_HPP
