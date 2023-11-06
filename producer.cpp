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

const bool brokerFull(const std::queue<Requests>& broker)
{
    return broker.size() > MAX_CRYPTO_REQUESTS;
}

void produceRequest(ProducerData* producerData)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(producerData->request_delay));
    Threading::safeAction(&producerData->brokerData.broker_mutex, [&](){
        producerData->brokerData.broker.push(producerData->request_type);
    });
    pthread_cond_signal(&producerData->brokerData.broker_monitor);
    // report_request_added(producerData->request_type);
}

// Does something
void* Producer::produce(void* arg)
{
    auto producerData = (ProducerData*)arg;
    while (producerData->total_requests < producerData->max_requests)
    {
        if (brokerFull(producerData->brokerData.broker))
        {
            pthread_cond_wait(&producerData->brokerData.broker_monitor, &producerData->brokerData.broker_mutex);
        }
        else
        {
            produceRequest(producerData);
        }
    }
}