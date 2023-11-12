/* 
 * @brief Facilitate multi-threaded functionality 
 *
 * Define some threading helper functions and child argument data structures in header.
 */

#include "parent_thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <thread>
#include <chrono>

// Helper to create all threads at the same time
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

// Delete thread memory at end of program
int ParentThread::cleanWorkerThreads(const ThreadArray& threads, const int numThreads)
{
    for (int i = 0; i < numThreads; i++)
    {
        delete(threads[i]);
    }
    return EXIT_SUCCESS;
}

pthread_t* Threading::spawnThread(void*(*function)(void*), void* args)
{
    pthread_t* thread = new pthread_t;
    pthread_create(thread, NULL, function, args);
    return thread;
}

// This will organize queue data based on request type to pass to reporting functions.
std::vector<unsigned int> SharedData::getQueueData(std::queue<Requests> q)
{
    const size_t queueSize = q.size();
    std::vector<unsigned int> dataArray(RequestTypeN);
    for (size_t i = 0; i < queueSize; i++) {
        const RequestType requestType = q.front(); q.pop();
        dataArray[requestType]++;
    }
    return dataArray;
}