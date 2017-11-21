#ifndef BARRIER_HPP
#define BARRIER_HPP

#include <mutex>
#include <condition_variable>

namespace threading
{

    class Barrier
    {
    public:
        Barrier(std::size_t threshold) :
            m_threshold(threshold),
            m_count(threshold),
            m_generation(0)
        {

        }

        Barrier(const Barrier&) = delete;
        Barrier(Barrier&&) = delete;

        void notify_all();

        void wait();

        template<typename Rep, typename Period>
        bool wait_for(const std::chrono::duration<Rep, Period>& rel_time);

        template<typename Clock, typename Duration>
        bool wait_until(const std::chrono::time_point<Clock, Duration>& timeout_time);

        template<typename Predicate>
        bool wait_until_condition(Predicate pred);

    private:
        std::mutex m_mutex;
        std::condition_variable m_cv;
        std::size_t m_threshold;
        std::size_t m_count;
        std::size_t m_generation;
    };

    inline void Barrier::wait()
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        std::size_t gen = m_generation;

        if (--m_count == 0)
        {
            m_generation++;
            m_count = m_threshold;
            m_cv.notify_all();
        }
        else
        {
            m_cv.wait(lock, [this, gen]() { return gen != m_generation; });
        }
    }

    template<typename Rep, typename Period>
    inline bool Barrier::wait_for(const std::chrono::duration<Rep, Period>& rel_time)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        std::size_t gen = m_generation;
        bool success = false;

        if (--m_count == 0)
        {
            m_generation++;
            m_count = m_threshold;
            m_cv.notify_all();
            success = true;
        }
        else
        {
            success = m_cv.wait_for(lock, rel_time, [this, gen]() { return gen != m_generation; });
        }

        if (!success)
        {
            m_count++;
        }

        return success;
    }

    template<typename Clock, typename Duration>
    inline bool Barrier::wait_until(const std::chrono::time_point<Clock, Duration>& timeout_time)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        std::size_t gen = m_generation;
        bool success = false;

        if (--m_count == 0)
        {
            m_generation++;
            m_count = m_threshold;
            m_cv.notify_all();
            success = true;
        }
        else
        {
            success = m_cv.wait_until(lock, timeout_time, [this, gen]() { return gen != m_generation; });
        }

        if (!success)
        {
            m_count++;
        }

        return success;
    }

    inline void Barrier::notify_all()
    {
        m_cv.notify_all();
    }

    template<typename Predicate>
    inline bool Barrier::wait_until_condition(Predicate pred)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        std::size_t gen = m_generation;
        bool success = false;

        if (--m_count == 0)
        {
            m_generation++;
            m_count = m_threshold;
            m_cv.notify_all();
            success = true;
        }
        else
        {
            m_cv.wait(lock, [this, gen, pred]() { return gen != m_generation || pred(); });

            if (gen != m_generation)
            {
                success = true;
            }
            else
            {
                success = false;
                m_count++;
            }
        }

        return success;
    }
}

#endif
