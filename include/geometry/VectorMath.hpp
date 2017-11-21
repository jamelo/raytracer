#ifndef VECTOR_MATH_HPP
#define VECTOR_MATH_HPP

#include <geometry/Vector.hpp>

namespace geometry
{

    template <typename T>
    inline Vector<T, 3> cross_product(const Vector<T, 3>& lhs, const Vector<T, 3>& rhs)
    {
        return Vector<T, 3>(std::array<T, 3>({
            lhs[1] * rhs[2] - lhs[2] * rhs[1],
            lhs[2] * rhs[0] - lhs[0] * rhs[2],
            lhs[0] * rhs[1] - lhs[1] * rhs[0]
        }));
    }

    template <typename T, size_t Dimensions>
    inline T abs(const Vector<T, Dimensions>& v)
    {
        return std::sqrt(v * v);
    }

    template <typename T, size_t Dimensions>
    inline Vector<T, Dimensions> normalize(const Vector<T, Dimensions>& v)
    {
        return v / abs(v);
    }

}

#endif
