#ifndef SURFACE_HPP
#define SURFACE_HPP

#include <graphics/Colour.hpp>

class Surface
{
public:
    Surface(const graphics::ColourRgb<float> colour, double difuseReflectance, double reflectance = 0,
            double transmittance = 0, double emittance = 0, double refractiveIndex = 1.0) :
        m_colour(colour),
        m_difuseReflectance(difuseReflectance),
        m_reflectance(reflectance),
        m_transmittance(transmittance),
        m_emittance(emittance),
        m_refractiveIndex(refractiveIndex)
    {

    }

    const graphics::ColourRgb<float>& colour() const
    {
        return m_colour;
    }

    double difuseReflectance() const
    {
        return m_difuseReflectance;
    }

    double reflectance() const {
        return m_reflectance;
    }

    double transmittance() const
    {
        return m_transmittance;
    }

    double emittance() const
    {
        return m_emittance;
    }

    double refractiveIndex() const
    {
        return m_refractiveIndex;
    }

private:
    graphics::ColourRgb<float> m_colour;
    double m_difuseReflectance;
    double m_reflectance;
    double m_transmittance;
    double m_emittance;
    double m_refractiveIndex;
};

#endif
