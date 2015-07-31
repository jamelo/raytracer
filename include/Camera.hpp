#ifndef Camera_HPP
#define Camera_HPP

#include <cstddef>
#include <memory>
#include <mutex>
#include <random>
#include <thread>
#include <tuple>
#include <vector>

#include <Image.hpp>
#include <Point.hpp>
#include <Ray.hpp>
#include <ThreadPool.hpp>

using namespace geometry;

struct AntiAliaserNone
{
    class iterator
    {
    private:
        int m_n;

    public:
        iterator(int n = 0) : m_n(n) { }
        iterator& operator++() { ++m_n; return *this; }
        bool operator!=(iterator rhs) { return m_n != rhs.m_n; }

        Vector2 operator*() const {
            return Vector2(0, 0);
        }
    };

    AntiAliaserNone(size_t) { }

    iterator begin() const { return iterator(0); }
    iterator end() const { return iterator(1); }

private:

    friend class iterator;
};

//TODO: consolidate random number generators
//TODO: properly seed random number generators

class AntiAliaserRandom
{
private:
    size_t m_aaAmount;

public:
    class iterator
    {
    private:
        int m_n;

    public:
        iterator(int n = 0) : m_n(n) { }
        iterator& operator++() { ++m_n; return *this; }
        bool operator!=(iterator rhs) { return m_n != rhs.m_n; }

        Vector2 operator*() const {
            thread_local std::mt19937 randgen;
            thread_local std::uniform_real_distribution<double> dist(-1.0, 1.0);
            return Vector2(dist(randgen), dist(randgen));
        }
    };

    AntiAliaserRandom(size_t _aaAmount = 1) :
        m_aaAmount(_aaAmount)
    { }

    iterator begin() const { return iterator(0); }
    iterator end() const { return iterator(m_aaAmount); }

    friend class iterator;
};

class Camera
{
private:
    Point3 m_location;
    Vector3 m_direction;
    Vector3 m_up;
    Vector2 m_sensorSize;
    double m_focalLength;
    size_t m_resolutionX;
    size_t m_resolutionY;

public:
    Camera(size_t resX, size_t resY, Point3 location, Vector3 direction, double focalLength = 1.0,
            Vector3 up = {0.0, 1.0, 0.0}) :
            m_location(location),
            m_direction(normalize(direction)),
            m_up(normalize(up)),
            m_focalLength(focalLength),
            m_resolutionX(resX),
            m_resolutionY(resY) {
        double aspectRatio = double(resX) / double(resY);
        m_sensorSize = Vector2(aspectRatio, 1.0);
    }

    template <typename Renderer, typename AntiAliaser = AntiAliaserRandom>
    std::tuple<TaskHandle, std::shared_ptr<Image<ColourRgb<float>>>> render(ThreadPool& pool, Renderer renderer, AntiAliaser antiAliaser = AntiAliaser(16)) const
    {
        auto image = std::make_shared<Image<ColourRgb<float>>>(m_resolutionX, m_resolutionY);

        //TODO: optimize

        TaskHandle taskHandle = pool.enqueueTask([=](const Problem& problem, const std::atomic<bool>& cancelled) {
            size_t x = 0;
            size_t y = problem[0];
            auto row = *(image->begin() + y);

            Vector2 halfSensor = m_sensorSize / 2;
            Vector3 right = cross_product(m_direction, m_up);
            Vector3 focalLengthDirection = m_direction * m_focalLength;

            double recipResX = 1.0 / m_resolutionX;
            double recipResY = 1.0 / m_resolutionY;
            double yf = -double(y * 2) * recipResY + 1.0;

            for (auto& pixel : row) {
                double xf = -double(x * 2) * recipResX + 1.0;

                size_t samples = 0;

                //Apply anti aliasing by generating vectors with slightly offset directions.
                for (const auto& aaOffsetVector : antiAliaser) {
                    double xfaa = (xf + aaOffsetVector.x() * recipResX) * halfSensor.x();
                    double yfaa = (yf + aaOffsetVector.y() * recipResY) * halfSensor.y();

                    Ray3 ray(m_location, xfaa * right + yfaa * m_up + focalLengthDirection);

                    pixel += renderer(ray);
                    samples++;
                }

                pixel *= (1.0 / samples);
                x++;
            }
        }, ProblemSpace(m_resolutionY));

        return std::make_tuple(std::move(taskHandle), std::move(image));
    }
};

#endif // Camera_HPP
