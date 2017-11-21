#ifndef SCENE_LOADER_HPP
#define SCENE_LOADER_HPP

#include <memory>
#include <string>

class Scene;

//TODO: create scene loader factory

class SceneLoader
{
public:
    static std::unique_ptr<SceneLoader> create();

    virtual builders::BuilderArgs load(const std::string& filename) = 0;

    std::unique_ptr<Scene> createScene();
};

#endif
