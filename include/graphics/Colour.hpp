#ifndef COLOUR_HPP
#define COLOUR_HPP

#include <cmath>
#include <cstdint>

#include <SFML/Graphics/Image.hpp>

#include <RandomGenerator.hpp>

//TODO: implement more colour models
//TODO: implement wavelength based colour model

namespace graphics
{

    namespace impl
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

    template <typename Target, typename Source>
    Target colour_cast(const Source& src);

    template <>
    inline ColourRgb<std::uint8_t> colour_cast(const ColourRgb<float>& src)
    {
        return ColourRgb<std::uint8_t>(
            impl::map_float_to_uint8(src.red()),
            impl::map_float_to_uint8(src.green()),
            impl::map_float_to_uint8(src.blue())
        );
    }

    template <>
    inline ColourRgb<float> colour_cast(const ColourRgb<std::uint8_t>& src)
    {
        return ColourRgb<float>(
            impl::map_uint8_to_float(src.red()),
            impl::map_uint8_to_float(src.green()),
            impl::map_uint8_to_float(src.blue())
        );
    }

    template <>
    inline sf::Color colour_cast(const ColourRgb<float>& src)
    {
        return sf::Color(
            impl::map_float_to_uint8(src.red()),
            impl::map_float_to_uint8(src.green()),
            impl::map_float_to_uint8(src.blue())
        );
    }

    template <>
    inline ColourRgb<float> colour_cast(const sf::Color& src)
    {
        return ColourRgb<float>(
            impl::map_uint8_to_float(src.r),
            impl::map_uint8_to_float(src.g),
            impl::map_uint8_to_float(src.b)
        );
    }


}

#endif
