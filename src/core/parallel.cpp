#include <cstdint>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <nanopt/core/parallel.h>

namespace nanopt {

class ParallelForLoop;

static auto shutdownThreads = false;
static ParallelForLoop* workList = nullptr;
static std::mutex m;
static std::condition_variable cv;
static std::vector<std::thread> threads;


class ParallelForLoop {
public:
  ParallelForLoop(std::function<void(std::int64_t)>&& func, std::int64_t count, int chunkSize = 1) noexcept
    : func1D(func), count(count), chunkSize(chunkSize)
  { }

  ParallelForLoop(std::function<void(const Vector2i&)>&& func, const Vector2i& count) noexcept
    : func2D(func), count(count.x * count.y), countX(count.x), chunkSize(1)
  { }

  bool isFinish() const {
    return nextIndex == count && activeThreads == 0;
  }

public:
  std::function<void(std::int64_t)> func1D;
  std::function<void(const Vector2i&)> func2D;
  std::int64_t count;
  std::int64_t nextIndex = 0;
  int countX;
  int chunkSize;
  int activeThreads = 0;
  ParallelForLoop* next = nullptr;
};

void workerThreadFunc() {
  std::unique_lock<std::mutex> lock(m);
  while (!shutdownThreads) {
    if (!workList) cv.wait(lock);
    else {
      auto& loop = *workList;
      auto beg = loop.nextIndex;
      auto end = std::min(loop.nextIndex + loop.chunkSize, loop.count);
      loop.nextIndex = end;
      if (loop.nextIndex == loop.count) workList = loop.next;
      ++loop.activeThreads;

      lock.unlock();
      if (loop.func1D) {
        for (auto i = beg; i < end; ++i)
          loop.func1D(i);
      } else {
        loop.func2D(Vector2i(beg % loop.countX, beg / loop.countX));
      }

      lock.lock();
      --loop.activeThreads;
    }
  }
}

void parallelInit() {
  auto maxThreads = (int)std::max(std::thread::hardware_concurrency(), 1u) - 1;
  for (auto i = 0; i < maxThreads; ++i)
    threads.emplace_back(workerThreadFunc);
}

void parallelCleanup() {
  {
    std::lock_guard<std::mutex> gurad(m);
    shutdownThreads = true;
  }
  cv.notify_all();
  for (auto& thread : threads)
    thread.join();
  threads.clear();
  shutdownThreads = false;
}

void parallelFor1D(std::function<void(std::int64_t)> func, std::int64_t count, int chunkSize) {
  if (threads.empty() || count < chunkSize) {
    for (std::int64_t i = 0; i < count; ++i)
      func(i);
    return;
  }

  ParallelForLoop loop(std::move(func), count, chunkSize);
  std::unique_lock<std::mutex> lock(m);
  loop.next = workList;
  workList = &loop;
  cv.notify_all();

  while (!loop.isFinish()) {
    auto beg = loop.nextIndex;
    auto end = std::min(loop.nextIndex + chunkSize, loop.count);
    loop.nextIndex = end;
    if (loop.nextIndex == loop.count)
      workList = loop.next;
    ++loop.activeThreads;
    lock.unlock();
    for (auto i = beg; i < end; ++i)
      loop.func1D(i);
    lock.lock();
    --loop.activeThreads;
  }
}

void parallelFor2D(std::function<void(const Vector2i&)> func, const Vector2i& count) {
  auto n = count.x * count.y;
  if (n == 0) return;

  if (threads.empty() || n == 1) {
    for (auto i = 0; i < n; ++i)
      func(Vector2i(i % count.x, i / count.x));
    return;
  }

  ParallelForLoop loop(std::move(func), count);
  std::unique_lock<std::mutex> lock(m);
  loop.next = workList;
  workList = &loop;
  cv.notify_all();

  while (!loop.isFinish()) {
    if (loop.nextIndex == loop.count) {
      lock.unlock();
      lock.lock();
    } else {
      auto beg = loop.nextIndex;
      ++loop.nextIndex;
      ++loop.activeThreads;
      if (loop.nextIndex == loop.count)
        workList = loop.next;
      lock.unlock();
      loop.func2D(Vector2i(beg % loop.countX, beg / loop.countX));
      lock.lock();
      --loop.activeThreads;
    }
  }
}

}
