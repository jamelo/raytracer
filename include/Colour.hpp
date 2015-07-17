#ifndef Colour_HPP
#define Colour_HPP

template <typename T>
class ColourRgb
{
private:
    T m_red;
    T m_green;
    T m_blue;

public:
    ColourRgb() = default;

    ColourRgb(T _red, T _green, T _blue) :
    m_red(_red),
    m_green(_green),
    m_blue(_blue)
    { }

    T red() const { return m_red; }
    T green() const { return m_green; }
    T blue() const { return m_blue; }

    //TODO: implement more operators

    ColourRgb<T>& operator+=(const ColourRgb<T>& rhs)
    {
        m_red += rhs.m_red;
        m_green += rhs.m_green;
        m_blue += rhs.m_blue;

        return *this;
    }

    ColourRgb<T> operator+(const ColourRgb<T>& rhs) const {
        return ColourRgb<T>(*this) += rhs;
    }

    ColourRgb<T>& operator*=(T rhs)
    {
        m_red *= rhs;
        m_green *= rhs;
        m_blue *= rhs;

        return *this;
    }

    ColourRgb<T> operator*(T rhs) const {
        return ColourRgb<T>(*this) *= rhs;
    }

    friend ColourRgb<T> operator*(T lhs, const ColourRgb<T>& rhs) {
        return rhs * lhs;
    }

    ColourRgb<T>& operator*=(const ColourRgb<T>& rhs) {
        m_red *= rhs.red();
        m_green *= rhs.green();
        m_blue *= rhs.blue();

        return *this;
    }

    ColourRgb<T> operator*(const ColourRgb<T>& rhs) const {
        return ColourRgb<T>(*this) *= rhs;
    }
};

#endif // Colour_HPP
