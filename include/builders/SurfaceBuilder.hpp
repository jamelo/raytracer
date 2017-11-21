#ifndef SURFACE_BUILDER_HPP
#define SURFACE_BUILDER_HPP

#include <builders/BuilderBase.hpp>
#include <Surface.hpp>

namespace builders
{

    class SurfaceBuilder : public BuilderBase<Surface>
    {
    public:
        SurfaceBuilder() {
            parameter("colour", ParamType::eColour, OPTIONAL, ParamTypes::Colour(0, 0, 0));
            parameter("difuse-reflectance", ParamType::eFloat, OPTIONAL, 1.0);
            parameter("reflectance", ParamType::eFloat, OPTIONAL, 0.0);
            parameter("transmittance", ParamType::eFloat, OPTIONAL, 0.0);
            parameter("emittance", ParamType::eFloat, OPTIONAL, 0.0);
            parameter("refractive-index", ParamType::eFloat, OPTIONAL, 1.0);
        }

    private:
        virtual std::shared_ptr<Surface> construct(const BuilderArgs& args) {
            const auto& colour = args.get<graphics::ColourRgb<float>>("colour");
            auto difuseReflectance = args.get<double>("difuse-reflectance");
            auto reflectance = args.get<double>("reflectance");
            auto transmittance = args.get<double>("transmittance");
            auto emittance = args.get<double>("emittance");
            auto refractiveIndex = args.get<double>("refractive-index");

            return std::make_shared<Surface>(colour, difuseReflectance, reflectance, transmittance, emittance, refractiveIndex);
        }
    };

}

#endif
