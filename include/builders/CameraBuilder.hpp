#ifndef CAMERA_BUILDER_HPP
#define CAMERA_BUILDER_HPP

#include <builders/BuilderBase.hpp>
#include <Camera.hpp>

namespace builders
{

    class CameraBuilder : public BuilderBase<Camera>
    {
    public:
        CameraBuilder() {
            parameter("resolution", ParamType::eImageSize, REQUIRED);
            parameter("location", ParamType::ePoint3, REQUIRED);
            parameter("direction", ParamType::eVector3, REQUIRED);
            parameter("roll", ParamType::eFloat, OPTIONAL, 0.0);
            parameter("focal-length", ParamType::eFloat, OPTIONAL, 1.0);
            parameter("samples-per-pixel", ParamType::eInteger, OPTIONAL, 64l);
        }

    private:
        virtual std::shared_ptr<Camera> construct(const BuilderArgs& args) {
            const auto& resolution = args.get<ParamTypes::ImageSize>("resolution");
            const auto& location = args.get<ParamTypes::Point3>("location");
            const auto& direction = args.get<ParamTypes::Vector3>("direction");
            const auto& roll = args.get<ParamTypes::Float>("roll");
            const auto& focalLength = args.get<ParamTypes::Float>("focal-length");
            const auto& samplesPerPixel = args.get<ParamTypes::Integer>("samples-per-pixel");

            return std::make_shared<Camera>(resolution, location, direction, roll, focalLength, samplesPerPixel);
        }
    };

}

#endif
