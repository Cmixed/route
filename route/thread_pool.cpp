/**
 * @file ThreadPool.hpp
 * @brief 线程池实现头文件（带异常处理和线程动态调整）
 * @author cmixed
 * @version 3.0
 */

#include "pch.hpp"

namespace route {

    struct PathEndpoints {
        int startVertex{}; ///< 起始顶点
        int endVertex{};   ///< 结束顶点
    };

    class ThreadPool {
    public:
        using Task = std::function<void()>; ///< 任务类型定义

        explicit ThreadPool(size_t initialThreadCount = 4) 
            : minThreads(1), maxThreads(100), currentThreads(initialThreadCount) {
            if (initialThreadCount < minThreads || initialThreadCount > maxThreads) {
                throw std::invalid_argument("Initial thread count out of bounds");
            }
            startThreads(initialThreadCount);
        }

        ~ThreadPool() {
            try {
                shutdown();
            } catch (const std::exception& e) {
                std::cerr << "Exception during shutdown: " << e.what() << std::endl;
            }
        }

        template<class F, class... Args>
        auto submit(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
            using ReturnType = std::invoke_result_t<F, Args...>;
            
            std::unique_lock<std::mutex> lock(queueMutex);
            if (shutdownFlag.load()) {
                throw std::runtime_error("Cannot submit task to shutdown ThreadPool");
            }

            auto task = std::make_shared<std::packaged_task<ReturnType()>>(
                [f = std::forward<F>(f), args = std::make_tuple(std::forward<Args>(args)...)]() mutable {
                    try {
                        std::apply(f, args);
                    } catch (const std::exception& e) {
                        std::cerr << "Task execution failed: " << e.what() << std::endl;
                        throw;
                    } catch (...) {
                        std::cerr << "Unknown exception in task execution" << std::endl;
                        throw;
                    }
                }
            );

            std::future<ReturnType> future = task->get_future();

            size_t taskId = nextTaskId++;
            taskExecutionTimes[taskId] = std::make_pair(std::chrono::steady_clock::now(), std::chrono::steady_clock::time_point{});
            
            tasks.emplace([task, taskId, this]() {
                auto start = std::chrono::steady_clock::now();
                try {
                    (*task)();
                } catch (...) {
                    // 确保即使任务抛出异常，执行时间也能正确记录
                }
                {
                    std::lock_guard<std::mutex> lock(executionTimesMutex);
                    taskExecutionTimes[taskId].second = std::chrono::steady_clock::now();
                }
            });

            // 动态增加线程（如果需要）
            checkAndAddThreads();

            condition.notify_one();
            lock.unlock();

            return future;
        }

        void shutdown() {
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                if (shutdownFlag.exchange(true)) {
                    return; // 已经关闭，直接返回
                }
            }
            condition.notify_all();

            for (auto& thread : threads) {
                if (thread.joinable()) {
                    try {
                        thread.join();
                    } catch (const std::system_error& e) {
                        std::cerr << "Error joining thread: " << e.what() << std::endl;
                    }
                }
            }
        }

        void pause() {
            std::unique_lock<std::mutex> lock(queueMutex);
            pausedFlag.store(true);
        }

        void resume() {
            std::unique_lock<std::mutex> lock(queueMutex);
            pausedFlag.store(false);
            condition.notify_all();
        }

        std::chrono::duration<double> getTaskExecutionTime(size_t taskId) const {
            std::lock_guard<std::mutex> lock(executionTimesMutex);
            auto it = taskExecutionTimes.find(taskId);
            if (it == taskExecutionTimes.end()) {
                throw std::out_of_range("Task ID not found");
            }
            return it->second.second - it->second.first;
        }

        std::unordered_map<size_t, std::chrono::duration<double>> getAllTaskExecutionTimes() const {
            std::lock_guard<std::mutex> lock(executionTimesMutex);
            std::unordered_map<size_t, std::chrono::duration<double>> result;
            for (const auto& [id, times] : taskExecutionTimes) {
                result[id] = times.second - times.first;
            }
            return result;
        }

        // 设置线程池的最小和最大线程数
        void setThreadLimits(size_t min, size_t max) {
            if (min < 1) min = 1;
            if (max > 1000) max = 1000;
            if (min > max) std::swap(min, max);

            std::unique_lock<std::mutex> lock(queueMutex);
            minThreads = min;
            maxThreads = max;

            // 确保当前线程数在新范围内
            if (currentThreads < minThreads) {
                size_t addCount = minThreads - currentThreads;
                addThreads(addCount);
            } else if (currentThreads > maxThreads) {
                size_t removeCount = currentThreads - maxThreads;
                removeThreads(removeCount);
            }
        }

    private:
        void startThreads(size_t const threadCount) {
            threads.reserve(threadCount);
            try {
                for (size_t i = 0; i < threadCount; ++i) {
                    threads.emplace_back([this]() { workerLoop(); });
                }
                currentThreads = threadCount;
            } catch (const std::exception& e) {
                // 清理已创建的线程
                for (auto& thread : threads) {
                    if (thread.joinable()) {
                        thread.join();
                    }
                }
                throw std::runtime_error("Failed to create threads: " + std::string(e.what()));
            }
        }

        void workerLoop() {
            while (true) {
                Task task;
                {
                    std::unique_lock<std::mutex> lock(queueMutex);
                    condition.wait(lock, [this]() {
                        return shutdownFlag.load() || !tasks.empty() || !pausedFlag.load();
                    });

                    if (shutdownFlag.load() && tasks.empty()) {
                        return;
                    }

                    if (!pausedFlag.load() && !tasks.empty()) {
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                }

                if (task) {
                    try {
                        task();
                    } catch (const std::exception& e) {
                        std::cerr << "Exception in worker thread: " << e.what() << std::endl;
                    } catch (...) {
                        std::cerr << "Unknown exception in worker thread" << std::endl;
                    }
                }

                // 记录线程空闲时间，用于动态减少线程
                auto now = std::chrono::steady_clock::now();
                lastActivityTime = now;

                // 检查是否需要减少线程
                checkAndRemoveThreads();
            }
        }

        void checkAndAddThreads() {
            std::unique_lock<std::mutex> lock(queueMutex);
            size_t queueSize = tasks.size();
            size_t currentThreadsCount = threads.size();

            if (queueSize > currentThreadsCount * 2 && currentThreadsCount < maxThreads && canAddThreads.load()) {
                size_t addCount = std::min(queueSize / 2, maxThreads - currentThreadsCount);
                addThreads(addCount);
            }
        }

        void checkAndRemoveThreads() {
            std::unique_lock<std::mutex> lock(queueMutex);
            auto now = std::chrono::steady_clock::now();
            auto idleTime = now - lastActivityTime;

            // 如果线程空闲超过60秒，且线程数超过最小值，则减少线程
            if (idleTime > std::chrono::seconds(60) && threads.size() > minThreads) {
                canAddThreads.store(false); // 暂时禁止添加新线程
                size_t removeCount = std::min(threads.size() - minThreads, size_t(1));
                removeThreads(removeCount);
            } else {
                canAddThreads.store(true); // 允许添加新线程
            }
        }

        void addThreads(size_t count) {
            for (size_t i = 0; i < count && threads.size() < maxThreads; ++i) {
                threads.emplace_back([this]() { workerLoop(); });
            }
            currentThreads = threads.size();
        }

        void removeThreads(size_t count) {
            // 标记线程为可销毁
            for (size_t i = 0; i < count && !threads.empty(); ++i) {
                threads.back().detach(); // 分离线程，让其自然结束
                threads.pop_back();
            }
            currentThreads = threads.size();
        }

        std::vector<std::thread> threads;
        std::queue<Task> tasks;
        std::mutex queueMutex;
        std::condition_variable condition;
        std::atomic<bool> shutdownFlag{ false };
        std::atomic<bool> pausedFlag{ false };
        std::atomic<bool> canAddThreads{ true }; // 是否允许添加新线程
        std::atomic<size_t> nextTaskId{ 0 };
        std::atomic<size_t> currentThreads{ 0 }; // 当前线程数
        size_t minThreads{ 1 }; // 最小线程数
        size_t maxThreads{ 100 }; // 最大线程数
        std::chrono::steady_clock::time_point lastActivityTime{ std::chrono::steady_clock::now() }; // 最后活动时间

        mutable std::mutex executionTimesMutex;
        std::unordered_map<size_t, std::pair<std::chrono::steady_clock::time_point, std::chrono::steady_clock::time_point>> taskExecutionTimes;
    };

} // namespace route