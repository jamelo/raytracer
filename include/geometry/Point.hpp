#ifndef POINT_HPP
#define POINT_HPP

#include <algorithm>
#include <cmath>
#include <functional>
#include <numeric>
#include <initializer_list>

#include <geometry/PointBase.hpp>
#include <geometry/Vector.hpp>

namespace geometry
{

    template <typename T, size_t Dimensions>
    class Point : public detail::point_base<T, Dimensions>
    {
    public:
        Point() = default;

        Point(const std::array<T, Dimensions>& components) :
            detail::point_base<T, Dimensions>(components)
        {

        }

        explicit Point(const detail::point_base<T, Dimensions>& data) :
            detail::point_base<T, Dimensions>(data)
        {

        }

        explicit operator Vector<T, Dimensions>()
        {
            return Vector<T, Dimensions>(static_cast<detail::point_base<T, Dimensions>>(*this()));
        }

        Point<T, Dimensions>& operator+=(const Vector<T, Dimensions>& rhs);
        Point<T, Dimensions>& operator-=(const Vector<T, Dimensions>& rhs);

        Point<T, Dimensions> operator+(const Vector<T, Dimensions>& rhs) const;
        Point<T, Dimensions> operator-(const Vector<T, Dimensions>& rhs) const;

        Vector<T, Dimensions> operator-(const Point<T, Dimensions>& rhs) const;

        bool operator==(const Point<T, Dimensions>& rhs);
        bool operator!=(const Point<T, Dimensions>& rhs);

        friend Point<T, Dimensions> operator+(const Vector<T, Dimensions>& lhs, const Point<T, Dimensions>& rhs) {
            return rhs + lhs;
        }
    };

    template <typename T>
    class Point2t : public Point<T, 2>
    {
    public:
        Point2t() = default;

        Point2t(const Point<T, 2>& v) :
                Point<T, 2>(v) {
        }

        Point2t(T x, T y) :
                Point<T, 2>({x, y}) {
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
    class Point3t : public Point<T, 3>
    {
    public:
        Point3t() = default;

        Point3t(const Point<T, 3>& v) :
            Point<T, 3>(v)
        {

        }

        Point3t(T x, T y, T z) :
            Point<T, 3>({x, y, z})
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

    using Point2 = Point2t<geo_type>;
    using Point3 = Point3t<geo_type>;

    template <typename T, size_t Dimensions>
    Point<T, Dimensions>& Point<T, Dimensions>::operator+=(const Vector<T, Dimensions>& rhs)
    {
        std::transform(this->begin(), this->end(), rhs.begin(), this->begin(), std::plus<T>());
        return *this;
    }

    template <typename T, size_t Dimensions>
    Point<T, Dimensions>& Point<T, Dimensions>::operator-=(const Vector<T, Dimensions>& rhs)
    {
        std::transform(this->begin(), this->end(), rhs.begin(), this->begin(), std::minus<T>());
        return *this;
    }

    template <typename T, size_t Dimensions>
    Point<T, Dimensions> Point<T, Dimensions>::operator+(const Vector<T, Dimensions>& rhs) const
    {
        return Point<T, Dimensions>(*this) += rhs;
    }

    template <typename T, size_t Dimensions>
    Point<T, Dimensions> Point<T, Dimensions>::operator-(const Vector<T, Dimensions>& rhs) const
    {
        return Point<T, Dimensions>(*this) -= rhs;
    }

    template <typename T, size_t Dimensions>
    Vector<T, Dimensions> Point<T, Dimensions>::operator-(const Point<T, Dimensions>& rhs) const
    {
        std::array<T, Dimensions> temp;
        std::transform(this->begin(), this->end(), rhs.begin(), temp.begin(), std::minus<T>());

        return Vector<T, Dimensions>(temp);
    }

    template <typename T, size_t Dimensions>
    bool Point<T, Dimensions>::operator==(const Point<T, Dimensions>& rhs)
    {
        return std::equal(this->begin(), this->end(), rhs.begin());
    }

    template <typename T, size_t Dimensions>
    bool Point<T, Dimensions>::operator!=(const Point<T, Dimensions>& rhs)
    {
        return !std::equal(this->begin(), this->end(), rhs.begin());
    }

}

#endif
