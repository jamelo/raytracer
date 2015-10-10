#ifndef SurfaceDescription_HPP
#define SurfaceDescription_HPP

#include <boost/optional.hpp>

#include <Colour.hpp>
#include <Image.hpp>

class SurfaceDescription
{
private:
    ColourRgb<float> m_colour;
    double m_difuseReflectance;
    double m_reflectance;
    double m_transmittance;
    double m_emittance;
    double m_refractiveIndex;
    //boost::optional<Image<ColourRgb<float>>> m_texture;

public:
    SurfaceDescription(const ColourRgb<float> _colour,
            double _difuseReflectance, double _reflectance = 0, double _transmittance = 0, double _emittance = 0,
            double _refractiveIndex = 1.0) :
            m_colour(_colour),
            m_difuseReflectance(_difuseReflectance),
            m_reflectance(_reflectance),
            m_transmittance(_transmittance),
            m_emittance(_emittance),
            m_refractiveIndex(_refractiveIndex)
    { }

    const ColourRgb<float>& colour() const { return m_colour; }
    double difuseReflectance() const { return m_difuseReflectance; }
    double reflectance() const { return m_reflectance; }
    double transmittance() const { return m_transmittance; }
    double emittance() const { return m_emittance; }
    double refractiveIndex() const { return m_refractiveIndex; }
};

#endif // SurfaceDescription_HPP
