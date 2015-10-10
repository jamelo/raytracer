#ifndef ThreadPool_HPP
#define ThreadPool_HPP

#include <array>
#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

#include <Barrier.hpp>
#include <Image.hpp>
#include <Semaphore.hpp>

class ProblemSpace;

class Problem
{
private:
    const ProblemSpace* m_problemSpace;
    std::array<unsigned int, 4> m_currentProblem;

    Problem(const ProblemSpace&  _problemSpace, const std::array<unsigned int, 4>& _currentProblem = {0, 0, 0, 0}) :
        m_problemSpace(&_problemSpace),
        m_currentProblem(_currentProblem)
    { }

public:
    Problem() :
        m_problemSpace(nullptr),
        m_currentProblem({0, 0, 0, 0})
    { }

    Problem& operator++();

    Problem operator++(int) {
        Problem it(*this);
        ++(*this);
        return it;
    }

    bool operator==(const Problem& rhs) const {
        return m_currentProblem == rhs.m_currentProblem;
    }

    bool operator!=(const Problem& rhs) const {
        return m_currentProblem != rhs.m_currentProblem;
    }

    unsigned int operator[](unsigned int dimensionId) const {
        return m_currentProblem[dimensionId];
    }

    friend class ProblemSpace;
};

class ProblemSpace
{
private:
    std::array<unsigned int, 4> m_dimensions;

public:
    typedef Problem iterator;
    typedef Problem const_iterator;

    ProblemSpace(unsigned int w, unsigned int x = 1, unsigned int y = 1, unsigned int z = 1) :
        m_dimensions({w, x, y, z})
    { }

    ProblemSpace::iterator begin() const {
        return ProblemSpace::iterator(*this, {0, 0, 0, 0});
    }

    ProblemSpace::iterator end() const {
        return ProblemSpace::iterator(*this, {0, 0, 0, m_dimensions[3]});
    }

    friend class Problem;
};

inline Problem& Problem::operator++() {
    auto& p = m_currentProblem;
    p[0] = (p[0] + 1) % m_problemSpace->m_dimensions[0];
    p[1] = (p[1] + (p[0] == 0)) % m_problemSpace->m_dimensions[1];
    p[2] = (p[2] + (p[0] == 0 && p[1] == 0)) % m_problemSpace->m_dimensions[2];
    p[3] = (p[3] + (p[0] == 0 && p[1] == 0 && p[2] == 0));

    return *this;
}


class Task
{
private:
    typedef std::function<void(Image<ColourRgb<float>>& result, const Problem& problem, const std::atomic<bool>& cancelled)> TaskFunction;

    TaskFunction m_function;
    ProblemSpace m_problemSpace;
    std::unique_ptr<std::mutex> m_statusMutex;
    std::unique_ptr<std::condition_variable> m_taskComplete;
    std::function<void(const Image<ColourRgb<float>>& result, bool success)> m_completeCallback;
    std::function<void(const Image<ColourRgb<float>>& result, const Problem&)> m_problemCallback;
    std::function<void(const Image<ColourRgb<float>>& result)> m_startCallback;
    std::atomic<bool> m_cancelled;
    bool m_completed;
    Image<ColourRgb<float>> m_result;

    void notifyComplete() {
        std::unique_lock<std::mutex> lock(*m_statusMutex);
        m_completed = true;
        m_taskComplete->notify_all();

        if (m_completeCallback) {
            m_completeCallback(m_result, !m_cancelled);
        }
    }

    void notifyProblem(const Problem& problem) {
        if (m_problemCallback) {
            m_problemCallback(m_result, problem);
        }
    }

    void notifyStarted() {
        if (m_startCallback) {
            m_startCallback(m_result);
        }
    }

    Task(Image<ColourRgb<float>>&& _image, const TaskFunction& _function, const ProblemSpace& _problemSpace) :
        m_function(_function),
        m_problemSpace(_problemSpace),
        m_statusMutex(std::make_unique<std::mutex>()),
        m_taskComplete(std::make_unique<std::condition_variable>()),
        m_cancelled(false),
        m_completed(false),
        m_result(std::move(_image))
    { }

public:
    Task(const Task&) = delete;
    Task& operator=(const Task&) = delete;

    Task(Task&& task) :
        m_function(std::move(task.m_function)),
        m_problemSpace(std::move(task.m_problemSpace)),
        m_statusMutex(std::move(task.m_statusMutex)),
        m_taskComplete(std::move(task.m_taskComplete)),
        m_cancelled(task.m_cancelled.load()),
        m_completed(std::move(task.m_completed)),
        m_result(std::move(task.m_result))
    { }


    Task& operator=(Task&& task) {
        m_function = std::move(task.m_function);
        m_problemSpace = std::move(task.m_problemSpace);
        m_statusMutex = std::move(task.m_statusMutex);
        m_taskComplete = std::move(task.m_taskComplete);
        m_cancelled = task.m_cancelled.load();
        m_completed = std::move(task.m_completed);
        m_result = std::move(task.m_result);

        return *this;
    }

    void wait() {
        std::unique_lock<std::mutex> lock(*m_statusMutex);
        m_taskComplete->wait(lock, [this]() { return m_completed; });
    }

    void cancel() {
        m_cancelled = true;
    }

    bool completed() {
        std::unique_lock<std::mutex> lock(*m_statusMutex);
        return m_completed;
    }

    bool cancelled() const {
        return m_cancelled;
    }

    const ProblemSpace& problemSpace() const {
        return m_problemSpace;
    }

    void operator()(ProblemSpace::iterator problem) {
        m_function(m_result, problem, m_cancelled);
    }

    void setCompleteCallback(std::function<void(const Image<ColourRgb<float>>& result, bool success)> func) {
        m_completeCallback = func;
    }

    void setProblemCallback(std::function<void(const Image<ColourRgb<float>>& result, const Problem&)> func) {
        m_problemCallback = func;
    }

    void setStartCallback(std::function<void(const Image<ColourRgb<float>>& result)> func) {
        m_startCallback = func;
    }

    const Image<ColourRgb<float>>& result() const {
        return m_result;
    }

    friend class ThreadPool;
};

class TaskHandle
{
private:
    std::shared_ptr<Task> m_task;

    TaskHandle(std::shared_ptr<Task> _task) :
        m_task(std::move(_task))
    { }

public:
    TaskHandle(const TaskHandle&) = delete;
    TaskHandle& operator=(const TaskHandle&) = delete;
    TaskHandle(TaskHandle&&) = default;
    TaskHandle& operator=(TaskHandle&&) = default;

    void wait() {
        m_task->wait();
    }

    void cancel() {
        m_task->cancel();
    }

    bool completed() {
        return m_task->completed();
    }

    void setCompleteCallback(std::function<void(const Image<ColourRgb<float>>& result, bool success)> func) {
        m_task->setCompleteCallback(func);
    }

    void setProblemCallback(std::function<void(const Image<ColourRgb<float>>& result, const Problem&)> func) {
        m_task->setProblemCallback(func);
    }

    void setStartCallback(std::function<void(const Image<ColourRgb<float>>& result)> func) {
        m_task->setStartCallback(func);
    }

    friend class ThreadPool;
};

class ThreadPool
{
private:
    std::vector<std::thread> m_threads;
    std::queue<std::shared_ptr<Task>> m_tasks;
    std::mutex m_threadCounterMutex;
    std::mutex m_problemMutex;
    std::mutex m_queueMutex;
    std::condition_variable m_tasksComplete;
    std::size_t m_threadsDoneCount;
    std::size_t m_totalThreads;
    std::atomic<bool> m_closeRequested;
    std::atomic<bool> m_closing;
    ProblemSpace::iterator m_currentProblem;
    Semaphore m_queueSemaphore;
    Barrier m_barrier;
    bool m_running;

    std::shared_ptr<Task> getNextTask() {
        std::unique_lock<std::mutex> counterLock(m_threadCounterMutex);
        m_threadsDoneCount++;

        if (m_threadsDoneCount >= m_totalThreads) {
            while (!m_queueSemaphore.wait_for(std::chrono::milliseconds(100))) {
                if (m_closeRequested) {
                    m_closing = true;
                    m_barrier.notify_all();
                    return nullptr;
                }
            }

            m_threadsDoneCount = 0;
            m_currentProblem = m_tasks.front()->problemSpace().begin();
        }

        counterLock.unlock();
        m_barrier.wait_until_condition([this]() { return m_closing.load(); });

        if (m_closing) {
            return nullptr;
        }

        return m_tasks.front();
    }

    void retireTask(std::shared_ptr<Task>& task) {
        std::unique_lock<std::mutex> counterLock(m_threadCounterMutex);
        m_threadsDoneCount++;

        if (m_threadsDoneCount >= m_totalThreads) {
            task->notifyComplete();
            m_threadsDoneCount = 0;

            std::unique_lock<std::mutex> queueLock(m_queueMutex);
            m_tasks.pop();

            if (m_tasks.empty()) {
                m_tasksComplete.notify_all();
            }
        }

        counterLock.unlock();
        m_barrier.wait();
    }

    void threadFunction() {
        while (true) {
            std::shared_ptr<Task> task = getNextTask();
            ProblemSpace::iterator problem;

            if (!task) {
                return;
            }

            task->notifyStarted();

            while (!task->cancelled()) {
                std::unique_lock<std::mutex> problemLock(m_problemMutex);

                if (m_currentProblem == task->problemSpace().end()) {
                    break;
                }

                problem = m_currentProblem++;
                problemLock.unlock();

                //Execute task on a single problem from the problem space
                (*task)(problem);
                task->notifyProblem(problem);
            }

            retireTask(task);
        }
    }

public:
    ThreadPool() :
        m_threadsDoneCount(0),
        m_totalThreads(std::thread::hardware_concurrency()),
        m_closeRequested(false),
        m_closing(false),
        m_queueSemaphore(0),
        m_barrier(std::thread::hardware_concurrency()),
        m_running(false)
    { }

    TaskHandle enqueueTask(Image<ColourRgb<float>>&& image, const Task::TaskFunction& function, const ProblemSpace& problemSpace) {
        auto task = std::make_shared<Task>(Task(std::move(image), function, problemSpace));
        TaskHandle handle(task);

        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_tasks.emplace(std::move(task));
        m_queueSemaphore.notify();

        if (!m_running) {
            for (unsigned int i = 0; i < std::thread::hardware_concurrency(); i++) {
                m_threads.emplace_back(std::bind(&ThreadPool::threadFunction, this));
            }

            m_running = true;
        }

        return handle;
    }

    void wait() {
        //std::unique_lock<std::mutex> lock(m_queueMutex);
        //m_tasksComplete.wait(lock, [this]() { return m_tasks.empty(); });
        m_closeRequested = true;

        for (auto& thread : m_threads) {
            thread.join();
        }
    }
};

#endif // ThreadPool_HPP
