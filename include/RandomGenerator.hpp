#ifndef RandomGenerator_HPP
#define RandomGenerator_HPP

#include <random>

class RandomGenerator
{
public:
    static std::mt19937& get_instance() {
        thread_local std::mt19937 randgen = []() {
            std::random_device rand;
            return std::mt19937(rand());
        }();

        return randgen;
    }
};

#endif // RandomGenerator_HPP
