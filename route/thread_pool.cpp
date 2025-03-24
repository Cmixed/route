#include "pch.hpp"

namespace route {

	struct PathEndpoints
	{
        int startVertex{};
        int endVertex{};
	};


    class ThreadPool {
    public:
        using Task = std::function<void()>;

        explicit ThreadPool(size_t threadCount) {
            startThreads(threadCount);
        }

        ~ThreadPool() {
            shutdown();
        }

        template<class F, class... Args>
        auto submit(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
            using ReturnType = std::invoke_result_t<F, Args...>;
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
            if (it != taskExecutionTimes.end()) {
                return it->second.second - it->second.first;
            }
            return std::chrono::duration<double>::zero();
        }

        std::unordered_map<size_t, std::chrono::duration<double>> getAllTaskExecutionTimes() const {
            std::lock_guard<std::mutex> lock(executionTimesMutex);
            std::unordered_map<size_t, std::chrono::duration<double>> result;
            for (const auto& [taskName, timePair] : taskExecutionTimes) {
                result[taskName] = timePair.second - timePair.first;
            }
            return result;
        }

    private:
        void startThreads(size_t const threadCount) {
            threads.reserve(threadCount);
            for (size_t i = 0; i < threadCount; ++i) {
                threads.emplace_back([this]() { workerLoop(); });
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
                    task();
                }
            }
        }

        std::vector<std::thread> threads;
        std::queue<Task> tasks;
        std::mutex queueMutex;
        std::condition_variable condition;
        std::atomic<bool> shutdownFlag{ false };
        std::atomic<bool> pausedFlag{ false };
        std::atomic<size_t> nextTaskId{ 0 };

        mutable std::mutex executionTimesMutex;
        std::unordered_map<size_t, std::pair<std::chrono::steady_clock::time_point, std::chrono::steady_clock::time_point>> taskExecutionTimes;
    };

}