#ifndef RAYTRACER_HPP
#define RAYTRACER_HPP

#include <memory>

#include <graphics/Colour.hpp>
#include <threading/ThreadPool.hpp>


class Scene;

threading::TaskHandle render(threading::ThreadPool& pool, const std::shared_ptr<Scene>& scene);

#endif
