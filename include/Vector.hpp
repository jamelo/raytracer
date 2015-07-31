#ifndef Vector_HPP
#define Vector_HPP

#include <algorithm>
#include <cmath>

#include <PointBase.hpp>

namespace geometry
{

    typedef double geo_type;

    template <typename T, size_t Dimensions>
    class Vector : public detail::point_base<T, Dimensions>
    {
    public:
        Vector() :
            detail::point_base<T, Dimensions>()
        { }

        Vector(const std::array<T, Dimensions>& _components) :
                detail::point_base<T, Dimensions>(_components) {
        }

        Vector<T, Dimensions> operator+() const {
            return *this;
        }

        Vector<T, Dimensions> operator-() const {
            Vector<T, Dimensions> result = *this;
            std::transform(result.begin(), result.end(), result.begin(), std::negate<T>());
            return result;
        }

        Vector<T, Dimensions>& operator+=(const Vector<T, Dimensions>& rhs) {
            std::transform(this->begin(), this->end(), rhs.begin(), this->begin(), std::plus<T>());
            return *this;
        }

        Vector<T, Dimensions>& operator-=(const Vector<T, Dimensions>& rhs) {
            std::transform(this->begin(), this->end(), rhs.begin(), this->begin(), std::minus<T>());
            return *this;
        }

        Vector<T, Dimensions>& operator*=(T rhs) {
            std::transform(this->begin(), this->end(), this->begin(), [=](T lhs) { return lhs * rhs; });
            return *this;
        }

        Vector<T, Dimensions>& operator/=(T rhs) {
            std::transform(this->begin(), this->end(), this->begin(), [=](T lhs) { return lhs / rhs; });
            return *this;
        }

        Vector<T, Dimensions> operator+(const Vector<T, Dimensions>& rhs) const {
            return Vector<T, Dimensions>(*this) += rhs;
        }

        Vector<T, Dimensions> operator-(const Vector<T, Dimensions>& rhs) const {
            return Vector<T, Dimensions>(*this) -= rhs;
        }

        Vector<T, Dimensions> operator*(T rhs) const {
            return Vector<T, Dimensions>(*this) *= rhs;
        }

        Vector<T, Dimensions> operator/(T rhs) const {
            return Vector<T, Dimensions>(*this) /= rhs;
        }

        T operator*(const Vector<T, Dimensions>& rhs) const {
            return std::inner_product(this->begin(), this->end(), rhs.begin(), 0.0);
        }

        bool operator==(const Vector<T, Dimensions>& rhs) {
            return std::equal(this->begin(), this->end(), rhs.begin());
        }

        bool operator!=(const Vector<T, Dimensions>& rhs) {
            return !std::equal(this->begin(), this->end(), rhs.begin());
        }

        friend Vector<T, Dimensions> operator*(T lhs, const Vector<T, Dimensions>& rhs) {
            return rhs * lhs;
        }
    };

    template <typename T>
    class Vector2t : public Vector<T, 2>
    {
    public:
        Vector2t() = default;

        Vector2t(const Vector<T, 2>& _v) :
                Vector<T, 2>(_v) {
        }

        Vector2t(T _x, T _y) :
                Vector<T, 2>({_x, _y}) {
        }

        T x() const { return (*this)[0]; }
        T y() const { return (*this)[1]; }
    };

    template <typename T>
    class Vector3t : public Vector<T, 3>
    {
    public:
        Vector3t() = default;

        Vector3t(const Vector<T, 3>& _v) :
                Vector<T, 3>(_v) {
        }

        Vector3t(T _x, T _y, T _z) :
                Vector<T, 3>({_x, _y, _z})
        { }

        T x() const { return (*this)[0]; }
        T y() const { return (*this)[1]; }
        T z() const { return (*this)[2]; }
    };

    typedef Vector2t<geo_type> Vector2;
    typedef Vector3t<geo_type> Vector3;

    template <typename T>
    inline Vector3t<T> cross_product(const Vector3t<T>& lhs, const Vector3t<T>& rhs)
    {
        return Vector3t<T>(
                lhs.y() * rhs.z() - lhs.z() * rhs.y(),
                lhs.z() * rhs.x() - lhs.x() * rhs.z(),
                lhs.x() * rhs.y() - lhs.y() * rhs.x()
        );
    }

    template <typename T, size_t Dimensions>
    inline T abs(const Vector<T, Dimensions>& v) {
        return std::sqrt(v * v);
    }

    template <typename T, size_t Dimensions>
    inline Vector<T, Dimensions> normalize(const Vector<T, Dimensions>& v) {
        return v / abs(v);
    }

}

#endif // Vector_HPP
