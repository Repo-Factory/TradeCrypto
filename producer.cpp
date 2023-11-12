/* 
 * @brief Place requests into the queue
 *
 * while we havent reached the limit of requests we will place items into the queue as long as it is not full. If it is, we will make the thread 
 * sleep using our condition variable 
 */

#include "producer.h"
#include "tradecrypto.h"
#include "parent_thread.h"
#include "report.h"
#include <thread>
#include <chrono>
#include <iostream>

// End condition for producers is when we have reached our n flag argument
const bool STILL_REQUESTS(Producer* producer_context)   
{
    return producer_context->total_requests < producer_context->max_requests;
}

// Condition to sleep producer thread
const bool BROKER_FULL(Producer* producer_context)      
{
    return producer_context->broker.size() >= MAX_CRYPTO_REQUESTS;
}

// Alternative condition to sleep producer thread to wait on bitcoin monitor
const bool BITCOIN_FULL(Producer* producer_context)     
{
    return SharedData::getQueueData(producer_context->broker)[Requests::Bitcoin] >= MAX_BITCOIN_REQUESTS;
}

// Place item into queue and update our metrics to print to terminal.
void produceRequest(Producer* producer_context)
{
    // Place item in queue
    if (!STILL_REQUESTS(producer_context)) return;
    producer_context->broker.push(producer_context->request_type);

    // Update metrics
    producer_context->total_requests++;
    producer_context->requests_produced[producer_context->request_type]++;
    report_request_added(producer_context->request_type, producer_context->requests_produced, SharedData::getQueueData(producer_context->broker).data());

    // Signal to consumers that an item was added
    pthread_cond_signal(&producer_context->general_monitor);
}

// Thread function that will execute producer functionality. 
void* ProducerThread::produce(void* arg)
{
    auto producer_context = (Producer*)arg;
    while (STILL_REQUESTS(producer_context))
    {
        pthread_mutex_lock(&producer_context->broker_mutex);
        // We will sleep if broker is full or if bitcoin is full and this is the bitcoin producer. We have to make sure to use the correct monitor
        // to wake up at the correct indication
        while (BROKER_FULL(producer_context)) {
            pthread_cond_wait(&producer_context->general_monitor, &producer_context->broker_mutex);
        } 
        while (BITCOIN_FULL(producer_context) && producer_context->request_type == Requests::Bitcoin) {
            pthread_cond_wait(&producer_context->bitcoin_monitor, &producer_context->broker_mutex);
        }
        // If queue isn't fully we can simply produce a request
        produceRequest(producer_context);
        pthread_mutex_unlock(&producer_context->broker_mutex);
        std::this_thread::sleep_for(std::chrono::milliseconds(producer_context->request_delay));
    }
    return NULL;
}