#ifndef Barrier_HPP
#define Barrier_HPP

#include <mutex>
#include <condition_variable>

class Barrier
{
private:
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::size_t m_threshold;
    std::size_t m_count;
    std::size_t m_generation;

public:
    Barrier(std::size_t _threshold) :
        m_threshold(_threshold),
        m_count(_threshold),
        m_generation(0)
    { }

    Barrier(const Barrier&) = delete;
    Barrier(Barrier&&) = delete;

    void wait() {
        std::unique_lock<std::mutex> lock(m_mutex);
        std::size_t gen = m_generation;

        if (--m_count == 0) {
            m_generation++;
            m_count = m_threshold;
            m_cv.notify_all();
        } else {
            m_cv.wait(lock, [this, gen]() { return gen != m_generation; });
        }
    }

    template<typename Rep, typename Period>
    bool wait_for(const std::chrono::duration<Rep, Period>& rel_time) {
        std::unique_lock<std::mutex> lock(m_mutex);
        std::size_t gen = m_generation;
        bool success = false;

        if (--m_count == 0) {
            m_generation++;
            m_count = m_threshold;
            m_cv.notify_all();
            success = true;
        } else {
            success = m_cv.wait_for(lock, rel_time, [this, gen]() { return gen != m_generation; });
        }

        if (!success) {
            m_count++;
        }

        return success;
    }

    template<typename Clock, typename Duration>
    bool wait_until(const std::chrono::time_point<Clock, Duration>& timeout_time) {
        std::unique_lock<std::mutex> lock(m_mutex);
        std::size_t gen = m_generation;
        bool success = false;

        if (--m_count == 0) {
            m_generation++;
            m_count = m_threshold;
            m_cv.notify_all();
            success = true;
        } else {
            success = m_cv.wait_until(lock, timeout_time, [this, gen]() { return gen != m_generation; });
        }

        if (!success) {
            m_count++;
        }

        return success;
    }

    void notify_all() {
        m_cv.notify_all();
    }

    template<typename Predicate>
    bool wait_until_condition(Predicate pred) {
        std::unique_lock<std::mutex> lock(m_mutex);
        std::size_t gen = m_generation;
        bool success = false;

        if (--m_count == 0) {
            m_generation++;
            m_count = m_threshold;
            m_cv.notify_all();
            success = true;
        } else {
            m_cv.wait(lock, [this, gen, pred]() { return gen != m_generation || pred(); });

            if (gen != m_generation) {
                success = true;
            } else {
                success = false;
                m_count++;
            }
        }

        return success;
    }

};

#endif // Barrier_HPP
