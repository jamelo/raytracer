#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <algorithm>
#include <cmath>
#include <numeric>

#include <geometry/PointBase.hpp>

namespace geometry
{

    typedef double geo_type;

    template <typename T, size_t Dimensions>
    class Vector : public detail::point_base<T, Dimensions>
    {
    public:
        Vector() :
            detail::point_base<T, Dimensions>()
        {

        }

        Vector(const std::array<T, Dimensions>& components) :
            detail::point_base<T, Dimensions>(components)
        {

        }

        explicit Vector(const detail::point_base<T, Dimensions>& data) :
            detail::point_base<T, Dimensions>(data)
        {

        }

        Vector<T, Dimensions> operator+() const;
        Vector<T, Dimensions> operator-() const;

        Vector<T, Dimensions> operator+(const Vector<T, Dimensions>& rhs) const;
        Vector<T, Dimensions> operator-(const Vector<T, Dimensions>& rhs) const;

        Vector<T, Dimensions> operator*(T rhs) const;
        Vector<T, Dimensions> operator/(T rhs) const;

        Vector<T, Dimensions>& operator+=(const Vector<T, Dimensions>& rhs);
        Vector<T, Dimensions>& operator-=(const Vector<T, Dimensions>& rhs);

        Vector<T, Dimensions>& operator*=(T rhs);
        Vector<T, Dimensions>& operator/=(T rhs);

        T operator*(const Vector<T, Dimensions>& rhs) const;

        bool operator==(const Vector<T, Dimensions>& rhs);
        bool operator!=(const Vector<T, Dimensions>& rhs);

        friend Vector<T, Dimensions> operator*(T lhs, const Vector<T, Dimensions>& rhs)
        {
            return rhs * lhs;
        }
    };

    template <typename T>
    class Vector2t : public Vector<T, 2>
    {
    public:
        Vector2t() = default;

        Vector2t(const Vector<T, 2>& _v) :
            Vector<T, 2>(_v)
        {

        }

        Vector2t(T _x, T _y) :
            Vector<T, 2>({_x, _y})
        {

        }

        T x() const
        {
            return (*this)[0];
        }

        T y() const
        {
            return (*this)[1];
        }
    };

    template <typename T>
    class Vector3t : public Vector<T, 3>
    {
    public:
        Vector3t() = default;

        Vector3t(const Vector<T, 3>& _v) :
            Vector<T, 3>(_v)
        {

        }

        Vector3t(T _x, T _y, T _z) :
            Vector<T, 3>({_x, _y, _z})
        {

        }

        T x() const
        {
            return (*this)[0];
        }

        T y() const
        {
            return (*this)[1];
        }

        T z() const
        {
            return (*this)[2];
        }
    };

    using Vector2 = Vector2t<geo_type>;
    using Vector3 = Vector3t<geo_type>;

    template <typename T, size_t Dimensions>
    Vector<T, Dimensions> Vector<T, Dimensions>::operator+() const
    {
        return *this;
    }

    template <typename T, size_t Dimensions>
    Vector<T, Dimensions> Vector<T, Dimensions>::operator-() const
    {
        Vector<T, Dimensions> result = *this;
        std::transform(result.begin(), result.end(), result.begin(), std::negate<T>());
        return result;
    }

    template <typename T, size_t Dimensions>
    Vector<T, Dimensions>& Vector<T, Dimensions>::operator+=(const Vector<T, Dimensions>& rhs)
    {
        std::transform(this->begin(), this->end(), rhs.begin(), this->begin(), std::plus<T>());
        return *this;
    }

    template <typename T, size_t Dimensions>
    Vector<T, Dimensions>& Vector<T, Dimensions>::operator-=(const Vector<T, Dimensions>& rhs)
    {
        std::transform(this->begin(), this->end(), rhs.begin(), this->begin(), std::minus<T>());
        return *this;
    }

    template <typename T, size_t Dimensions>
    Vector<T, Dimensions>& Vector<T, Dimensions>::operator*=(T rhs)
    {
        std::transform(this->begin(), this->end(), this->begin(), [=](T lhs) { return lhs * rhs; });
        return *this;
    }

    template <typename T, size_t Dimensions>
    Vector<T, Dimensions>& Vector<T, Dimensions>::operator/=(T rhs)
    {
        std::transform(this->begin(), this->end(), this->begin(), [=](T lhs) { return lhs / rhs; });
        return *this;
    }

    template <typename T, size_t Dimensions>
    Vector<T, Dimensions> Vector<T, Dimensions>::operator+(const Vector<T, Dimensions>& rhs) const
    {
        return Vector<T, Dimensions>(*this) += rhs;
    }

    template <typename T, size_t Dimensions>
    Vector<T, Dimensions> Vector<T, Dimensions>::operator-(const Vector<T, Dimensions>& rhs) const
    {
        return Vector<T, Dimensions>(*this) -= rhs;
    }

    template <typename T, size_t Dimensions>
    Vector<T, Dimensions> Vector<T, Dimensions>::operator*(T rhs) const
    {
        return Vector<T, Dimensions>(*this) *= rhs;
    }

    template <typename T, size_t Dimensions>
    Vector<T, Dimensions> Vector<T, Dimensions>::operator/(T rhs) const
    {
        return Vector<T, Dimensions>(*this) /= rhs;
    }

    template <typename T, size_t Dimensions>
    T Vector<T, Dimensions>::operator*(const Vector<T, Dimensions>& rhs) const
    {
        return std::inner_product(this->begin(), this->end(), rhs.begin(), 0.0);
    }

    template <typename T, size_t Dimensions>
    bool Vector<T, Dimensions>::operator==(const Vector<T, Dimensions>& rhs)
    {
        return std::equal(this->begin(), this->end(), rhs.begin());
    }

    template <typename T, size_t Dimensions>
    bool Vector<T, Dimensions>::operator!=(const Vector<T, Dimensions>& rhs)
    {
        return !std::equal(this->begin(), this->end(), rhs.begin());
    }

}

#include <geometry/VectorMath.hpp>

#endif
