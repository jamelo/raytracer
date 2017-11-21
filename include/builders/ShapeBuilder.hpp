#ifndef SHAPE_BUILDER_HPP
#define SHAPE_BUILDER_HPP

#include <algorithm>
#include <boost/optional.hpp>
#include <iterator>
#include <map>

#include <builders/BuilderBase.hpp>
#include <builders/CustomShapeBuilder.hpp>
#include <builders/SurfaceBuilder.hpp>
#include <shapes/Shape.hpp>

namespace builders
{

    class ShapeBuilder : public BuilderBase<shapes::Shape>
    {
    public:
        struct Registration
        {
            Registration(const std::string& name, std::unique_ptr<CustomShapeBuilder>&& builder)
            {
                if (ShapeBuilder::shapeBuilderRegistry().find(name) != ShapeBuilder::shapeBuilderRegistry().end())
                {
                    throw ShapeRedefinitionException(name);
                }

                ShapeBuilder::shapeBuilderRegistry()[name] = std::move(builder);
            }
        };

        ShapeBuilder()
        {
            setAllowExtraArguments(true);
            parameter("shape", ParamType::eString, REQUIRED);
        }

        void setSurfaces(const ParamTypes::SurfaceMap& surfaces)
        {
            m_surfaces = surfaces;
        }

    private:
        static std::map<std::string, std::unique_ptr<CustomShapeBuilder>>& shapeBuilderRegistry()
        {
            static std::map<std::string, std::unique_ptr<CustomShapeBuilder>> s_shapeBuilderRegistry;
            return s_shapeBuilderRegistry;
        }

        virtual std::shared_ptr<shapes::Shape> construct(const BuilderArgs& args) override
        {
            const auto& shapeType = args.get<std::string>("shape");

            auto builderIter = shapeBuilderRegistry().find(shapeType);

            if (builderIter == shapeBuilderRegistry().end())
            {
                throw UnknownShapeTypeException(shapeType);
            }

            CustomShapeBuilder& builder = *builderIter->second;
            builder.setSurfaceMapGetter([&]() -> const ParamTypes::SurfaceMap& { return m_surfaces; });
            BuilderArgs shapeArgs;

            std::copy_if(args.begin(), args.end(), std::inserter(shapeArgs, shapeArgs.end()),
                [](const auto& value) { return value.first != std::string("shape"); }
            );

            return builder.build(shapeArgs);
        }

        ParamTypes::SurfaceMap m_surfaces;
    };

}

#endif
