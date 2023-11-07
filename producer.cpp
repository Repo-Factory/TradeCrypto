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

void produceRequest(Producer* producerData)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(producerData->request_delay));
    Threading::safeAction(&producerData->broker_mutex, [&](){
        producerData->broker.push(producerData->request_type);
    });
    producerData->total_requests++;
    producerData->requests_produced[producerData->request_type]++;
    pthread_cond_signal(&producerData->broker_monitor);
    report_request_added(producerData->request_type, producerData->requests_produced, getQueueData(producerData->broker));
}

// Does something
void* ProducerThread::produce(void* arg)
{
    auto producerData = (Producer*)arg;
    while (producerData->total_requests < producerData->max_requests)
    {
        if (producerData->broker.size() > MAX_CRYPTO_REQUESTS)
        {
            pthread_cond_wait(&producerData->broker_monitor, &producerData->broker_mutex);
        }
        else
        {
            produceRequest(producerData);
        }
    }
    return NULL;
}