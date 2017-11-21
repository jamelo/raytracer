#ifndef SCENE_HPP
#define SCENE_HPP

#include <map>
#include <memory>
#include <string>

#include <builders/ParamTypes.hpp>
#include <Camera.hpp>
#include <shapes/Shape.hpp>
#include <Surface.hpp>

class Scene
{
public:
    using ShapeListType = std::vector<std::shared_ptr<shapes::Shape>>;

    Scene(const std::string& title, const std::string& description, const Camera& camera,
            const ShapeListType& geometry) :
        m_title(title),
        m_description(description),
        m_camera(camera),
        m_geometry(geometry)
    {
        for (auto& shape : m_geometry)
        {
            if (shape->surface().emittance() > 0.0)
            {
                m_lights.push_back(shape);
            }
        }
    }

    const Camera& camera() const
    {
        return m_camera;
    }

    const ShapeListType& geometry() const
    {
        return m_geometry;
    }

    const ShapeListType& lights() const
    {
        return m_lights;
    }

private:
    std::string m_title;
    std::string m_description;
    Camera m_camera;
    ShapeListType m_geometry;
    ShapeListType m_lights;
};

#endif
