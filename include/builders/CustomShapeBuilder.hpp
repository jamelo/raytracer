#ifndef CUSTOM_SHAPE_BUILDER_H
#define CUSTOM_SHAPE_BUILDER_H

#include <builders/BuilderBase.hpp>
#include <builders/SurfaceBuilder.hpp>
#include <shapes/Shape.hpp>

namespace builders
{

    class CustomShapeBuilder : public BuilderBase<shapes::Shape>
    {
    public:
        CustomShapeBuilder() {
            setAllowExtraArguments(true);
        }

        void setSurfaceMapGetter(const std::function<const ParamTypes::SurfaceMap&()>& surfaceGetter) {
            m_surfaceGetter = surfaceGetter;
        }

    private:
        virtual ParamValue customConvert(const ParamValue& arg, ParamType targetType) override final {
            switch (targetType) {
                case ParamType::eSurface:
                    return getSurface(arg);
                default:
                    throw InvalidConversionException(arg, targetType);
            }
        }

        std::shared_ptr<Surface> getSurface(const ParamValue& arg) {
            ParamType argType = getParamType(arg);

            // TODO: change to eIdentifier
            if (argType == ParamType::eString) {
                auto surfaceName = IdentifierString(boost::get<std::string>(arg));
                auto surfIter = m_surfaceGetter().find(surfaceName);

                if (surfIter == m_surfaceGetter().end()) {
                    throw UndeclaredIdentifierException(surfaceName);
                }

                return surfIter->second;
            } else if (argType == ParamType::eObject) {
                return SurfaceBuilder().build(*boost::get<ParamTypes::Object>(arg));
            } else {
                throw InvalidConversionException(arg, ParamType::eSurface);
            }
        }

        std::function<const ParamTypes::SurfaceMap&()> m_surfaceGetter;
    };

}

#endif
