#ifndef Camera_HPP
#define Camera_HPP

#include <cstddef>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

#include <Image.hpp>
#include <Point.hpp>
#include <Ray.hpp>

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

    iterator begin() const { return iterator(0); }
    iterator end() const { return iterator(1); }

private:

    friend class iterator;
};

template <size_t AaAmount>
struct AntiAliaserRandom
{
    static_assert(AaAmount > 0, "AaAmount must be greater than zero.");

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

    iterator begin() const { return iterator(0); }
    iterator end() const { return iterator(AaAmount); }

private:

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

    template <typename Renderer, typename AntiAliaser>
    Image<ColourRgb<float>> render(Renderer renderer, AntiAliaser antiAliaser) const
    {
        Image<ColourRgb<float>> image(m_resolutionX, m_resolutionY);
        Vector2 halfSensor = m_sensorSize / 2;

        double recipResX = 1.0 / m_resolutionX;
        double recipResY = 1.0 / m_resolutionY;

        Vector3 right = cross_product(m_direction, m_up);
        Vector3 focalLengthDirection = m_direction * m_focalLength;

        //TODO: optimize

        size_t y = 0;
        auto rowIter = image.begin();

        std::mutex rowMutex;
        std::vector<std::thread> threads;

        for (size_t t = 0; t < std::thread::hardware_concurrency(); t++) {
            threads.emplace_back([=, &y, &rowIter, &rowMutex, &image]() {
                size_t localY;
                decltype(*rowIter) row;

                for (;;) {
                    {
                        std::unique_lock<std::mutex> lock(rowMutex);

                        if (rowIter == image.end()) {
                            return;
                        }

                        row = *rowIter;
                        rowIter++;
                        localY = y;
                        y++;
                    }

                    double yf = double(localY * 2) * recipResY - 1.0;
                    size_t x = 0;

                    for (auto& pixel : row) {
                        double xf = double(x * 2) * recipResX - 1.0;

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
                }
            });
        }

        /*for (auto row : image) {



            y++;
        }*/

        for (auto& thread : threads) {
            thread.join();
        }

        return image;
    }
};

#endif // Camera_HPP
