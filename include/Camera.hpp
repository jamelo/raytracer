#ifndef Camera_HPP
#define Camera_HPP

#include <cstddef>
#include <memory>
#include <mutex>
#include <random>
#include <thread>
#include <tuple>
#include <vector>

#include <geometry/Point.hpp>
#include <geometry/Ray.hpp>
#include <graphics/Image.hpp>
#include <threading/ThreadPool.hpp>
#include <RandomGenerator.hpp>

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
            thread_local std::uniform_real_distribution<double> dist(-1.0, 1.0);
            return Vector2(dist(RandomGenerator::get_instance()), dist(RandomGenerator::get_instance()));
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
    size_t m_antiAliasingAmount;

public:
    Camera(size_t resX, size_t resY, Point3 location, Vector3 direction, double focalLength = 1.0,
            Vector3 up = {0.0, 1.0, 0.0}, size_t antiAliasingAmount = 32) :
            m_location(location),
            m_direction(normalize(direction)),
            m_up(normalize(up)),
            m_focalLength(focalLength),
            m_resolutionX(resX),
            m_resolutionY(resY),
            m_antiAliasingAmount(antiAliasingAmount) {
        double aspectRatio = double(resX) / double(resY);
        m_sensorSize = Vector2(aspectRatio, 1.0);
    }

    Camera(Point2t<int64_t> resolution, Point3 location, Vector3 direction, double roll, double focalLength, int64_t samplesPerPixel) :
        m_location(location),
        m_direction(normalize(direction)),
        m_up({0.0, 1.0, 0.0}),
        m_focalLength(focalLength),
        m_resolutionX(resolution.x()),
        m_resolutionY(resolution.y()),
        m_antiAliasingAmount(samplesPerPixel) {
            (void)roll;
            double aspectRatio = double(m_resolutionX) / double(m_resolutionY);
        m_sensorSize = Vector2(aspectRatio, 1.0);
    }

    template <typename Renderer, typename AntiAliaser = AntiAliaserRandom>
    threading::TaskHandle render(threading::ThreadPool& pool, Renderer renderer) const
    {
        AntiAliaser antiAliaser = AntiAliaser(m_antiAliasingAmount);
        auto image = graphics::Image<graphics::ColourRgb<float>>(m_resolutionX, m_resolutionY);

        Camera c(*this);

        //TODO: optimize

        threading::TaskHandle taskHandle = pool.enqueueTask(std::move(image), [=](graphics::Image<graphics::ColourRgb<float>>& result, const threading::Problem& problem, const std::atomic<bool>& cancelled) {
            (void)cancelled;

            size_t x = 0;
            size_t y = problem[0];
            auto row = *(result.begin() + y);

            Vector2 halfSensor = c.m_sensorSize / 2;
            Vector3 right = cross_product(c.m_direction, c.m_up);
            Vector3 focalLengthDirection = c.m_direction * c.m_focalLength;

            double recipResX = 1.0 / c.m_resolutionX;
            double recipResY = 1.0 / c.m_resolutionY;
            double yf = -double(y * 2) * recipResY + 1.0;

            for (auto& pixel : row) {
                double xf = -double(x * 2) * recipResX + 1.0;

                int samples = 0;

                //Apply anti aliasing by generating vectors with slightly offset directions.
                for (const auto& aaOffsetVector : antiAliaser) {
                    double xfaa = (xf + aaOffsetVector.x() * recipResX) * halfSensor.x();
                    double yfaa = (yf + aaOffsetVector.y() * recipResY) * halfSensor.y();

                    Ray3 ray(c.m_location, geometry::normalize(xfaa * right + yfaa * c.m_up + focalLengthDirection));

                    pixel += renderer(ray);
                    samples++;
                }

                pixel *= (1.0 / samples);
                x++;
            }
        }, threading::ProblemSpace(c.m_resolutionY));

        return taskHandle;
    }
};

#endif // Camera_HPP
