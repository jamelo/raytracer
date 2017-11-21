#ifndef RAY_HPP
#define RAY_HPP

#include <geometry/Point.hpp>
#include <geometry/Vector.hpp>

namespace geometry
{

    template <typename T, size_t Dimensions>
    class Ray
    {
    public:
        Ray(const Point<T, Dimensions>& origin, const Vector<T, Dimensions>& direction) :
            m_origin(origin),
            m_direction(direction)
        {

        }

        const Point<T, Dimensions>& origin() const
        {
            return m_origin;
        }

        const Vector<T, Dimensions>& direction() const
        {
            return m_direction;
        }

        Point<T, Dimensions>& operator()(T t) const
        {
            return m_origin + m_direction * t;
        }

    private:
        Point<T, Dimensions> m_origin;
        Vector<T, Dimensions> m_direction;
    };

    using Ray2 = Ray<geo_type, 2>;
    using Ray3 = Ray<geo_type, 3>;

}

#endif
