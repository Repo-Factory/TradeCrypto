/* 
 * @brief Take out requests from the queue
 *
 * while we havent reached the limit of requests we will take out items from the queue is long as it is not empty. If it is, we will make the thread 
 * sleep using our condition variable 
 */

#include "consumer.h"
#include "tradecrypto.h"
#include "parent_thread.h"
#include "report.h"
#include <thread>
#include <chrono>

// Since we have been keeping track of the number of each request consumed, we can add them together to see if we've consumed the last one.
const int getTotalRequestsConsumed(unsigned int** requests_consumed)
{
    return requests_consumed[0][0] + requests_consumed[0][1] + requests_consumed[1][0] + requests_consumed[1][1];
}

// Take a request out of the queue and update our metrics to be printed to the console. We will return if we consume last request to communicate to loop
const bool consumeRequest(Consumer* consumer_context)
{
    // Get first element
    const RequestType type = consumer_context->broker.front();
    consumer_context->broker.pop();

    // Update and print metrics
    consumer_context->requests_consumed[consumer_context->ledger][type]++;
    report_request_removed(consumer_context->ledger, type, consumer_context->requests_consumed[consumer_context->ledger], SharedData::getQueueData(consumer_context->broker).data());
    
    // Signal to producer threads a new slot is available using appropriate monitor
    if (type == Requests::Bitcoin) pthread_cond_signal(&consumer_context->bitcoin_monitor);
    pthread_cond_signal(&consumer_context->general_monitor);
    return getTotalRequestsConsumed(consumer_context->requests_consumed) == consumer_context->max_requests;
}

// Thread function that will execute consumer functionality. When we have reached our max requests, we will inform the parent thread using a semaphore barrier.
void* ConsumerThread::consume(void* arg)
{
    auto consumer_context = (Consumer*)arg;
    while (!consumer_context->barrier_triggered)
    {
        pthread_mutex_lock(&consumer_context->broker_mutex);
        // Sleep thread if queue is empty, pass in broker mutex to temporarily unlock and allow other threads to continue 
        while (consumer_context->broker.empty()) {   
            pthread_cond_wait(&consumer_context->general_monitor, &consumer_context->broker_mutex);
        } 
        // Otherwise we will simply consume a request   
        consumer_context->barrier_triggered = consumeRequest(consumer_context);
        pthread_mutex_unlock(&consumer_context->broker_mutex);
        std::this_thread::sleep_for(std::chrono::milliseconds(consumer_context->request_delay));
    }
    sem_post(&consumer_context->barrier);
    return NULL;
}
