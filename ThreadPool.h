#pragma once

#include <condition_variable>
#include <cstddef>
#include <exception>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <utility>
#include <vector>

class ThreadPool {
 private:
  std::vector<std::thread> workers;
  std::queue<std::function<void()>> tasks;

  bool stop;
  std::mutex queueMutex;
  std::condition_variable condition;

 public:
  ThreadPool(size_t numberOfThreads = std::thread::hardware_concurrency())
      : stop(false) {
    if (numberOfThreads == 0) numberOfThreads = 4;

    for (size_t i = 0; i < numberOfThreads; ++i) {
      workers.emplace_back([this] {
        while (true) {
          std::unique_lock<std::mutex> lock(queueMutex);
          condition.wait(lock, [this] { return stop || !tasks.empty(); });

          if (stop && tasks.empty()) return;

          auto task = std::move(tasks.front());
          tasks.pop();

          lock.unlock();
          try {
            task();
          } catch (const std::exception& e) {
            std::cerr << "ThreadPool task error: " << e.what() << std::endl;
          }
        }
      });
    }
  }

  ThreadPool(const ThreadPool&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;
  ThreadPool(ThreadPool&&) = delete;
  ThreadPool& operator=(ThreadPool&&) = delete;

  template <class F>
  void enqueue(F&& task) {
    {
      std::unique_lock<std::mutex> lock(queueMutex);
      tasks.emplace(std::forward<F>(task));
    }
    condition.notify_one();
  }

  ~ThreadPool() {
    {
      std::unique_lock<std::mutex> lock(queueMutex);
      stop = true;
    }
    condition.notify_all();
    for (std::thread& worker : workers) worker.join();
  }
};
