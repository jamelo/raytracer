#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include <mutex>
#include <condition_variable>

namespace threading
{

    class Semaphore
    {
    private:
        std::mutex m_mutex;
        std::condition_variable m_cv;
        std::size_t m_count;

    public:
        Semaphore (std::size_t count = 0) :
            m_count(count)
        {

        }

        Semaphore(const Semaphore&) = delete;
        Semaphore(Semaphore&&) = delete;

        void notify();

        void wait();

        template<typename Rep, typename Period>
        bool wait_for(const std::chrono::duration<Rep, Period>& rel_time);

        template<typename Clock, typename Duration>
        bool wait_until(const std::chrono::time_point<Clock, Duration>& timeout_time);
    };

    inline void Semaphore::notify()
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_count++;
        m_cv.notify_one();
    }

    inline void Semaphore::wait()
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_cv.wait(lock, [this]() { return m_count > 0; });
        m_count--;
    }

    template<typename Rep, typename Period>
    inline bool Semaphore::wait_for(const std::chrono::duration<Rep, Period>& rel_time)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        bool success = m_cv.wait_for(lock, rel_time, [this]() { return m_count > 0; });

        if (success)
        {
            m_count--;
        }

        return success;
    }

    template<typename Clock, typename Duration>
    inline bool Semaphore::wait_until(const std::chrono::time_point<Clock, Duration>& timeout_time)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        bool success = m_cv.wait_until(lock, timeout_time, [this]() { return m_count > 0; });

        if (success)
        {
            m_count--;
        }

        return success;
    }

}

#endif
