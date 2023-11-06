#ifndef THREADING_H
#define THREADING_H
#include <array>
#include <pthread.h>
#include <functional>
#include <atomic>
#include <queue>
#include "tradecrypto.h"

constexpr const int MAX_CRYPTO_REQUESTS = 15;
constexpr const int MAX_BITCOIN_REQUESTS = 6;

constexpr const int NUM_CHILD_THREADS = 4;
typedef std::array<pthread_t*, NUM_CHILD_THREADS> ThreadArray;

struct ThreadData
{
    void*(*threadFunction)(void*);
    void* threadArgs;
};

struct BrokerData
{
    std::queue<Requests> broker;
    pthread_mutex_t broker_mutex;
    pthread_cond_t broker_monitor;
};

struct ProducerData
{
    BrokerData& brokerData;
    const Requests request_type;
    int& total_requests;
    const int max_requests;
    const int request_delay;
};

struct ConsumerData
{
    BrokerData& brokerData;
    const Requests request_type;
    const Consumers ledger;
    const int request_delay;
};

namespace Threading
{
    pthread_t* spawnThread(void*(*function)(void*), void* args);
    void safeAction(pthread_mutex_t* mutex, std::function<void()> action);
}

namespace ParentThread
{
    ThreadArray spawnWorkerThreads(const ThreadData* threadData[], const int numThreads);
    int cleanWorkerThreads(const ThreadArray& threads, const int numThreads);
}

#endif

/*  ConsumerData(std::queue<Requests> broker, pthread_mutex_t mutex, pthread_cond_t monitor, const Requests request_type)
     : broker(broker),
       broker_access(mutex),
       broker_monitor(monitor),
       request_type(request_type)
    {}; */