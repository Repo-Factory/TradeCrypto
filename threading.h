#ifndef THREADING_H
#define THREADING_H
#include <array>
#include <pthread.h>
#include <functional>
#include <atomic>

constexpr const int NUM_CHILD_THREADS = 4;
typedef std::array<pthread_t*, NUM_CHILD_THREADS> ThreadArray;

namespace Threading
{
    pthread_t* spawnThread(void*(*function)(void*), void* args);
    void safeAction(pthread_mutex_t* mutex, std::function<void()> action);
}

struct ThreadData
{
    void*(*threadFunction)(void*);
    void* threadArgs;
};

namespace ParentThread
{
    ThreadArray spawnWorkerThreads(const ThreadData* threadData[], const int numThreads);
    int cleanWorkerThreads(const ThreadArray& threads, const int numThreads);
}

#endif
