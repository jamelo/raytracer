#ifndef QUATERNION_HPP
#define QUATERNION_HPP

template <typename T>
class Quaternion
{
public:
    Quaternion();

    Quaternion<T> operator+() const;
    Quaternion<T> operator-() const;
    Quaternion<T> operator+(const Quaternion<T>& rhs) const;
    Quaternion<T> operator-(const Quaternion<T>& rhs) const;
    Quaternion<T> operator*(const Quaternion<T>& rhs) const;
    Quaternion<T>& operator+=(const Quaternion<T>& rhs);
    Quaternion<T>& operator-=(const Quaternion<T>& rhs);
    Quaternion<T>& operator*=(const Quaternion<T>& rhs);
};

#endif
