/**
 * @file ThreadPool.hpp
 * @brief 线程池实现头文件
 * @author cmixed
 * @version 1.0
 */

#include "pch.hpp"

namespace route {

    /**
     * @struct PathEndpoints
     * @brief 路径端点结构体，用于存储起始和结束顶点
     */
    struct PathEndpoints
    {
        int startVertex{}; ///< 起始顶点
        int endVertex{};   ///< 结束顶点
    };

    /**
     * @class ThreadPool
     * @brief 线程池类，用于管理线程和任务队列
     */
    class ThreadPool {
    public:
        using Task = std::function<void()>; ///< 任务类型定义

        /**
         * @brief 构造函数，创建指定数量的线程
         * @param threadCount 线程数量
         */
        explicit ThreadPool(size_t threadCount) {
            startThreads(threadCount);
        }

        /**
         * @brief 析构函数，关闭线程池并等待所有线程结束
         */
        ~ThreadPool() {
            shutdown();
        }

        /**
         * @brief 提交任务到线程池
         * @tparam F 函数类型
         * @tparam Args 参数类型
         * @param f 函数对象
         * @param args 参数
         * @return 返回任务结果的future对象
         */
        template<class F, class... Args>
        auto submit(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
            using ReturnType = std::invoke_result_t<F, Args...>; ///< 返回类型
            auto task = std::make_shared<std::packaged_task<ReturnType()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );
            std::future<ReturnType> future = task->get_future();

            std::unique_lock<std::mutex> lock(queueMutex);
            if (shutdownFlag.load()) {
                throw std::runtime_error("submit on shutdown ThreadPool");
            }

            size_t taskId = nextTaskId++;
            taskExecutionTimes[taskId] = std::make_pair(std::chrono::steady_clock::now(), std::chrono::steady_clock::time_point{});
            tasks.emplace([task, taskId, this]() {
                auto start = std::chrono::steady_clock::now();
                (*task)();
                {
                    std::lock_guard<std::mutex> lock(executionTimesMutex);
                    taskExecutionTimes[taskId].second = std::chrono::steady_clock::now();
                }
                });

            condition.notify_one();
            lock.unlock();

            return future;
        }

        /**
         * @brief 关闭线程池，等待所有任务完成并销毁线程
         */
        void shutdown() {
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                shutdownFlag.store(true);
            }
            condition.notify_all();

            for (auto& thread : threads) {
                thread.join();
            }
        }

        /**
         * @brief 暂停线程池，停止新任务的执行
         */
        void pause() {
            std::unique_lock<std::mutex> lock(queueMutex);
            pausedFlag.store(true);
        }

        /**
         * @brief 恢复线程池，继续执行任务
         */
        void resume() {
            std::unique_lock<std::mutex> lock(queueMutex);
            pausedFlag.store(false);
            condition.notify_all();
        }

        /**
         * @brief 获取指定任务的执行时间
         * @param taskId 任务ID
         * @return 任务执行时间
         */
        std::chrono::duration<double> getTaskExecutionTime(size_t taskId) const {
            std::lock_guard<std::mutex> lock(executionTimesMutex);
            auto it = taskExecutionTimes.find(taskId);
            if (it != taskExecutionTimes.end()) {
                return it->second.second - it->second.first;
            }
            return std::chrono::duration<double>::zero();
        }

        /**
         * @brief 获取所有任务的执行时间
         * @return 包含任务ID和执行时间的映射
         */
        std::unordered_map<size_t, std::chrono::duration<double>> getAllTaskExecutionTimes() const {
            std::lock_guard<std::mutex> lock(executionTimesMutex);
            std::unordered_map<size_t, std::chrono::duration<double>> result;
            for (const auto& [taskName, timePair] : taskExecutionTimes) {
                result[taskName] = timePair.second - timePair.first;
            }
            return result;
        }

    private:
        /**
         * @brief 创建指定数量的线程
         * @param threadCount 线程数量
         */
        void startThreads(size_t const threadCount) {
            threads.reserve(threadCount);
            for (size_t i = 0; i < threadCount; ++i) {
                threads.emplace_back([this]() { workerLoop(); });
            }
        }

        /**
         * @brief 工作线程循环，处理任务队列中的任务
         */
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
                    task();
                }
            }
        }

        std::vector<std::thread> threads; ///< 线程容器
        std::queue<Task> tasks; ///< 任务队列
        std::mutex queueMutex; ///< 任务队列互斥量
        std::condition_variable condition; ///< 条件变量，用于线程间同步
        std::atomic<bool> shutdownFlag{ false }; ///< 关闭标志
        std::atomic<bool> pausedFlag{ false }; ///< 暂停标志
        std::atomic<size_t> nextTaskId{ 0 }; ///< 下一个任务ID

        mutable std::mutex executionTimesMutex; ///< 执行时间互斥量
        std::unordered_map<size_t, std::pair<std::chrono::steady_clock::time_point, std::chrono::steady_clock::time_point>> taskExecutionTimes; ///< 任务执行时间映射
    };

} // namespace route