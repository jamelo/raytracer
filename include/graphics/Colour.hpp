#ifndef COLOUR_HPP
#define COLOUR_HPP

#include <algorithm>
#include <cmath>
#include <cstdint>

#include <SFML/Graphics/Image.hpp>

#include <RandomGenerator.hpp>

//TODO: implement more colour models
//TODO: implement wavelength based colour model

namespace graphics
{

    namespace priv
    {

        template <typename T>
        inline T clamp(T value, T low, T high)
        {
            return std::max(std::min(value, high), low);
        }

        inline float map_uint8_to_float(uint8_t colour, float gamma = 2.2)
        {
            return std::pow(colour / 255.0f, gamma);
        }

        inline uint8_t map_float_to_uint8(float colour, float gamma = 2.2)
        {
            thread_local std::uniform_real_distribution<float> dist(-0.5/255, 0.5/255);
            return uint8_t(std::floor(clamp((std::pow(colour, 1.0f / gamma) + dist(RandomGenerator::get_instance())) * 256.0f, 0.0f, 255.0f)));
        }

    }

    template <typename T>
    class ColourRgb
    {
    public:
        ColourRgb() = default;

        ColourRgb(T _red, T _green, T _blue) :
            m_red(_red),
            m_green(_green),
            m_blue(_blue)
        {

        }

        T red() const
        {
            return m_red;
        }

        T green() const
        {
            return m_green;
        }

        T blue() const
        {
            return m_blue;
        }

        ColourRgb<T> operator+(const ColourRgb<T>& rhs) const;
        ColourRgb<T> operator*(const ColourRgb<T>& rhs) const;
        ColourRgb<T> operator*(T rhs) const;

        ColourRgb<T>& operator+=(const ColourRgb<T>& rhs);
        ColourRgb<T>& operator*=(const ColourRgb<T>& rhs);
        ColourRgb<T>& operator*=(T rhs);

        T max() const;
        T average() const;

        friend ColourRgb<T> operator*(T lhs, const ColourRgb<T>& rhs)
        {
            return rhs * lhs;
        }

    private:
        T m_red;
        T m_green;
        T m_blue;
    };

    template <typename T>
    inline ColourRgb<T>& ColourRgb<T>::operator+=(const ColourRgb<T>& rhs)
    {
        m_red += rhs.m_red;
        m_green += rhs.m_green;
        m_blue += rhs.m_blue;

        return *this;
    }

    template <typename T>
    inline ColourRgb<T> ColourRgb<T>::operator+(const ColourRgb<T>& rhs) const
    {
        return ColourRgb<T>(*this) += rhs;
    }

    template <typename T>
    inline ColourRgb<T>& ColourRgb<T>::operator*=(T rhs)
    {
        m_red *= rhs;
        m_green *= rhs;
        m_blue *= rhs;

        return *this;
    }

    template <typename T>
    inline ColourRgb<T> ColourRgb<T>::operator*(T rhs) const
    {
        return ColourRgb<T>(*this) *= rhs;
    }

    template <typename T>
    inline ColourRgb<T>& ColourRgb<T>::operator*=(const ColourRgb<T>& rhs)
    {
        m_red *= rhs.red();
        m_green *= rhs.green();
        m_blue *= rhs.blue();

        return *this;
    }

    template <typename T>
    inline ColourRgb<T> ColourRgb<T>::operator*(const ColourRgb<T>& rhs) const
    {
        return ColourRgb<T>(*this) *= rhs;
    }

    template <typename T>
    inline T ColourRgb<T>::max() const
    {
        return std::max({m_red, m_green, m_blue});
    }

    template <typename T>
    inline T ColourRgb<T>::average() const
    {
        return (m_red + m_green + m_blue) * (1.0 / 3.0);
    }

    template <typename Target, typename Source>
    Target colour_cast(const Source& src);

    template <>
    inline ColourRgb<std::uint8_t> colour_cast(const ColourRgb<float>& src)
    {
        return ColourRgb<std::uint8_t>(
            priv::map_float_to_uint8(src.red()),
            priv::map_float_to_uint8(src.green()),
            priv::map_float_to_uint8(src.blue())
        );
    }

    template <>
    inline ColourRgb<float> colour_cast(const ColourRgb<std::uint8_t>& src)
    {
        return ColourRgb<float>(
            priv::map_uint8_to_float(src.red()),
            priv::map_uint8_to_float(src.green()),
            priv::map_uint8_to_float(src.blue())
        );
    }

    template <>
    inline sf::Color colour_cast(const ColourRgb<float>& src)
    {
        return sf::Color(
            priv::map_float_to_uint8(src.red()),
            priv::map_float_to_uint8(src.green()),
            priv::map_float_to_uint8(src.blue())
        );
    }

    template <>
    inline ColourRgb<float> colour_cast(const sf::Color& src)
    {
        return ColourRgb<float>(
            priv::map_uint8_to_float(src.r),
            priv::map_uint8_to_float(src.g),
            priv::map_uint8_to_float(src.b)
        );
    }


}

#endif
