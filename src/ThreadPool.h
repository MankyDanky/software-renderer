#pragma once
#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class ThreadPool {
public:
    explicit ThreadPool(size_t numThreads) : stop(false), activeJobs(0) {
        for (size_t i = 0; i < numThreads; i++) {
            workers.emplace_back([this] {WorkerLoop();});
        }
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        condition.notify_all();
        for (auto& worker : workers) {
            worker.join();
        }
    }

    void Enqueue(std::function<void()> task) {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            tasks.push(std::move(task));
            activeJobs++;
        }
        condition.notify_one();
    }

    void WaitAll() {
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            doneCondition.wait(lock, [this] {
                return activeJobs == 0 && tasks.empty();
            });
        }
    }

    size_t GetThreadCount() const {return workers.size();}
private:
    void WorkerLoop() {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                condition.wait(lock, [this] {
                    return stop || !tasks.empty();
                });

                if (stop && tasks.empty()) {
                    return;
                }

                task = std::move(tasks.front());
                tasks.pop();
            }

            task();

            {
                std::unique_lock<std::mutex> lock(queueMutex);
                activeJobs--;
            }
            doneCondition.notify_all();
        }
    }

    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queueMutex;
    std::condition_variable condition;
    std::condition_variable doneCondition;

    std::atomic<bool> stop;
    std::atomic<int> activeJobs;
};