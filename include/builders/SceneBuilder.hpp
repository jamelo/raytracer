#ifndef SCENE_BUILDER_HPP
#define SCENE_BUILDER_HPP

#include <set>

#include <builders/BuilderBase.hpp>
#include <builders/CameraBuilder.hpp>
#include <builders/ShapeBuilder.hpp>
#include <Scene.hpp>

namespace builders
{

    class SceneBuilder : public BuilderBase<Scene>
    {
    public:
        SceneBuilder() :
            m_surfacesConstructed(false) {
            parameter("title", ParamType::eString, REQUIRED);
            parameter("description", ParamType::eString, OPTIONAL, "");
            parameter("camera", ParamType::eCamera, REQUIRED);
            parameter("Surfaces", ParamType::eSurfaceMap, OPTIONAL, ParamTypes::SurfaceMap());
            parameter("geometry", ParamType::eShapeMap, REQUIRED);
        }

    private:
        virtual std::shared_ptr<Scene> construct(const BuilderArgs& args) override {
            const auto& title = args.get<ParamTypes::String>("title");
            const auto& description = args.get<ParamTypes::String>("description");
            const auto& camera = *args.get<ParamTypes::Camera>("camera");
            const auto& geometryMap = args.get<ParamTypes::ShapeMap>("geometry");

            std::vector<std::shared_ptr<shapes::Shape>> geometry;
            std::transform(geometryMap.begin(), geometryMap.end(), std::back_inserter(geometry), [](auto& s){
                return std::move(s.second);
            });

            return std::make_shared<Scene>(title, description, camera, geometry);
        }

        virtual ParamValue customConvert(const ParamValue& arg, ParamType targetType) override {
            switch (targetType) {
                case ParamType::eCamera:
                    return createCamera(arg);
                case ParamType::eShapeMap:
                    return createShapeMap(arg);
                case ParamType::eSurfaceMap:
                    return createSurfaceMap(arg);
                default:
                    throw InvalidConversionException(arg, targetType);
            }
        }

        ParamValue createCamera(const ParamValue& arg) {
            const auto& cameraArgs = boost::get<ParamTypes::Object>(arg);
            return m_cameraBuilder.build(*cameraArgs);
        }

        ParamValue createShapeMap(const ParamValue& arg) {
            const auto& shapeEntries = boost::get<ParamTypes::Object>(arg);

            if (!m_surfacesConstructed) {
                throw SomeKindOfException();
            }

            ParamTypes::ShapeMap geometry;

            for (const auto& shapeEntry : *shapeEntries) {
                const ParamTypes::Identifier& shapeName = shapeEntry.first;
                const ParamTypes::Object& shapeArgs = boost::get<ParamTypes::Object>(shapeEntry.second);

                if (m_definedNames.find(shapeName) != m_definedNames.end()) {
                    throw SomeKindOfNameConflictException();
                }

                m_definedNames.insert(shapeName);
                geometry[shapeName] = m_shapeBuilder.build(*shapeArgs);
            }

            return geometry;
        }

        ParamValue createSurfaceMap(const ParamValue& arg) {
            const auto& surfaceEntries = boost::get<ParamTypes::Object>(arg);
            ParamTypes::SurfaceMap surfaces;

            for (const auto& surfaceEntry : *surfaceEntries) {
                const ParamTypes::Identifier& surfaceName = surfaceEntry.first;
                const ParamTypes::Object& surfaceArgs = boost::get<ParamTypes::Object>(surfaceEntry.second);

                if (m_definedNames.find(surfaceName) != m_definedNames.end()) {
                    throw SomeKindOfNameConflictException();
                }

                m_definedNames.insert(surfaceName);
                surfaces[surfaceName] = m_surfaceBuilder.build(*surfaceArgs);
            }

            m_surfacesConstructed = true;
            m_shapeBuilder.setSurfaces(surfaces);

            return surfaces;
        }

        std::set<ParamTypes::Identifier> m_definedNames;

        SurfaceBuilder m_surfaceBuilder;
        ShapeBuilder m_shapeBuilder;
        CameraBuilder m_cameraBuilder;

        bool m_surfacesConstructed;
    };

}

#endif
