#ifndef THREADING_H
#define THREADING_H
#include <array>
#include <pthread.h>
#include <functional>
#include <atomic>
#include <queue>
#include "semaphore.h"
#include "tradecrypto.h"

constexpr const int MAX_CRYPTO_REQUESTS = 15;
constexpr const int MAX_BITCOIN_REQUESTS = 6;
constexpr const int NUM_CHILD_THREADS = 4;

typedef std::array<pthread_t*, NUM_CHILD_THREADS> ThreadArray;

struct ThreadContext
{
    void*(*threadFunction)(void*);
    void* threadArgs;
};

struct Producer
{
    std::queue<Requests>& broker;
    pthread_mutex_t& broker_mutex;
    pthread_cond_t& broker_monitor;
    unsigned int* requests_produced;
    unsigned int& total_requests;
    const Requests request_type;
    const int max_requests;
    const int request_delay;
};

struct Consumer
{
    std::queue<Requests>& broker;
    pthread_mutex_t& broker_mutex;
    pthread_cond_t& broker_monitor;
    sem_t& barrier;
    unsigned int** requests_consumed;
    const Requests request_type;
    const Consumers ledger;
    const int max_requests;
    const int request_delay;
};

namespace Threading
{
    pthread_t* spawnThread(void*(*function)(void*), void* args);
    void safeAction(pthread_mutex_t* mutex, std::function<void()> action);
}

namespace ParentThread
{
    ThreadArray spawnWorkerThreads(const ThreadContext* threadData[], const int numThreads);
    int cleanWorkerThreads(const ThreadArray& threads, const int numThreads);
}

unsigned int* getQueueData(std::queue<Requests> q);

#endif
