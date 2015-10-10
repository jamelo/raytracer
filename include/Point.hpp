#ifndef Point_HPP
#define Point_HPP

#include <algorithm>
#include <cmath>
#include <functional>
#include <numeric>
#include <initializer_list>

#include <PointBase.hpp>
#include <Vector.hpp>

namespace geometry
{

    template <typename T, size_t Dimensions>
    class Point : public detail::point_base<T, Dimensions>
    {
    public:
        Point() = default;

        Point(const std::array<T, Dimensions>& _components) :
                detail::point_base<T, Dimensions>(_components) {
        }

        explicit Point(const detail::point_base<T, Dimensions>& _data) :
                detail::point_base<T, Dimensions>(_data) {
        }

        explicit operator Vector<T, Dimensions>() {
            return Vector<T, Dimensions>(static_cast<detail::point_base<T, Dimensions>>(*this()));
        }

        Point<T, Dimensions>& operator+=(const Vector<T, Dimensions>& rhs) {
            std::transform(this->begin(), this->end(), rhs.begin(), this->begin(), std::plus<T>());
            return *this;
        }

        Point<T, Dimensions>& operator-=(const Vector<T, Dimensions>& rhs) {
            std::transform(this->begin(), this->end(), rhs.begin(), this->begin(), std::minus<T>());
            return *this;
        }

        Point<T, Dimensions> operator+(const Vector<T, Dimensions>& rhs) const {
            return Point<T, Dimensions>(*this) += rhs;
        }

        Point<T, Dimensions> operator-(const Vector<T, Dimensions>& rhs) const {
            return Point<T, Dimensions>(*this) -= rhs;
        }

        Vector<T, Dimensions> operator-(const Point<T, Dimensions>& rhs) const {
            std::array<T, Dimensions> temp;
            std::transform(this->begin(), this->end(), rhs.begin(), temp.begin(), std::minus<T>());

            return Vector<T, Dimensions>(temp);
        }

        bool operator==(const Point<T, Dimensions>& rhs) {
            return std::equal(this->begin(), this->end(), rhs.begin());
        }

        bool operator!=(const Point<T, Dimensions>& rhs) {
            return !std::equal(this->begin(), this->end(), rhs.begin());
        }

        friend Point<T, Dimensions> operator+(const Vector<T, Dimensions>& lhs, const Point<T, Dimensions>& rhs) {
            return rhs + lhs;
        }
    };

    template <typename T>
    class Point2t : public Point<T, 2>
    {
    public:
        Point2t() = default;

        Point2t(const Point<T, 2>& _v) :
                Point<T, 2>(_v) {
        }

        Point2t(T _x, T _y) :
                Point<T, 2>({_x, _y}) {
        }

        T x() const { return (*this)[0]; }
        T y() const { return (*this)[1]; }
    };

    template <typename T>
    class Point3t : public Point<T, 3>
    {
    public:
        Point3t() = default;

        Point3t(const Point<T, 3>& _v) :
                Point<T, 3>(_v) {
        }

        Point3t(T _x, T _y, T _z) :
                Point<T, 3>({_x, _y, _z})
        { }

        T x() const { return (*this)[0]; }
        T y() const { return (*this)[1]; }
        T z() const { return (*this)[2]; }
    };

    typedef Point2t<geo_type> Point2;
    typedef Point3t<geo_type> Point3;

}

#endif // Point_HPP
