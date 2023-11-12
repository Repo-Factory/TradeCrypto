/* 
 * @brief 
 *
 * 
 * 
 * 
 */

#include "producer.h"
#include "tradecrypto.h"
#include "parent_thread.h"
#include "report.h"
#include <thread>
#include <chrono>
#include <iostream>

const bool STILL_REQUESTS(Producer* producer_context)   
{
    return producer_context->total_requests < producer_context->max_requests;
}

const bool BROKER_FULL(Producer* producer_context)      
{
    return producer_context->broker.size() >= MAX_CRYPTO_REQUESTS;
}

const bool BITCOIN_FULL(Producer* producer_context)     
{
    return SharedData::getQueueData(producer_context->broker)[Requests::Bitcoin] >= MAX_BITCOIN_REQUESTS;
}

void produceRequest(Producer* producer_context)
{
    if (!STILL_REQUESTS(producer_context)) return;
    producer_context->broker.push(producer_context->request_type);
    producer_context->total_requests++;
    producer_context->requests_produced[producer_context->request_type]++;
    report_request_added(producer_context->request_type, producer_context->requests_produced, SharedData::getQueueData(producer_context->broker).data());
    pthread_cond_signal(&producer_context->general_monitor);
}

void* ProducerThread::produce(void* arg)
{
    auto producer_context = (Producer*)arg;
    while (STILL_REQUESTS(producer_context))
    {
        pthread_mutex_lock(&producer_context->broker_mutex);
        while (BROKER_FULL(producer_context)) {
            pthread_cond_wait(&producer_context->general_monitor, &producer_context->broker_mutex);
        } 
        while (BITCOIN_FULL(producer_context) && producer_context->request_type == Requests::Bitcoin) {
            pthread_cond_wait(&producer_context->bitcoin_monitor, &producer_context->broker_mutex);
        }
        produceRequest(producer_context);
        pthread_mutex_unlock(&producer_context->broker_mutex);
        std::this_thread::sleep_for(std::chrono::milliseconds(producer_context->request_delay));
    }
    return NULL;
}