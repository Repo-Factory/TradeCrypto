/* 
 * @brief 
 *
 * 
 * 
 * 
 */

#include "parent_thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <thread>
#include <chrono>

ThreadArray ParentThread::spawnWorkerThreads(const ThreadContext* threadData[], const int numThreads)
{
    ThreadArray threads;
    for (int i = 0; i < numThreads; i++)
    {
        threads[i] = NULL;
    }
    for (int i = 0; i < numThreads; i++)
    {
        threads[i] = Threading::spawnThread(threadData[i]->threadFunction, threadData[i]->threadArgs);
    }
    return threads;
}

int ParentThread::cleanWorkerThreads(const ThreadArray& threads, const int numThreads)
{
    for (int i = 0; i < numThreads; i++)
    {
        delete(threads[i]);
    }
    return EXIT_SUCCESS;
}

void Threading::safeAction(pthread_mutex_t* mutex, const std::function<void()> performAction) 
{
    pthread_mutex_lock(mutex);
    performAction();
    pthread_mutex_unlock(mutex);
}

pthread_t* Threading::spawnThread(void*(*function)(void*), void* args)
{
    pthread_t* thread = new pthread_t;
    pthread_create(thread, NULL, function, args);
    return thread;
}

unsigned int* SharedData::getQueueData(std::queue<Requests> q)
{
    const size_t queueSize = q.size();
    unsigned int* dataArray = new unsigned int[RequestTypeN];
    for (size_t i = 0; i < queueSize; i++) {
        const RequestType requestType = q.front(); q.pop();
        dataArray[requestType]++;
    }
    return dataArray;
}