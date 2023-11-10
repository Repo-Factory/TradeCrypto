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

const bool STILL_REQUESTS(Producer* producer_context)   {return producer_context->total_requests < producer_context->max_requests;}
const bool BROKER_FULL(Producer* producer_context)      {return producer_context->broker.size() >= MAX_CRYPTO_REQUESTS;}

void produceRequest(Producer* producer_context)
{
    Threading::safeAction(&producer_context->broker_mutex, [&](){
        producer_context->broker.push(producer_context->request_type);
    });
    pthread_cond_signal(&producer_context->broker_monitor);
    producer_context->total_requests++;
    producer_context->requests_produced[producer_context->request_type]++;
    report_request_added(producer_context->request_type, producer_context->requests_produced, SharedData::getQueueData(producer_context->broker));
    std::this_thread::sleep_for(std::chrono::milliseconds(producer_context->request_delay));
}

void* ProducerThread::produce(void* arg)
{
    auto producer_context = (Producer*)arg;
    while (STILL_REQUESTS(producer_context))
    {
        if (BROKER_FULL(producer_context)) {
            Threading::safeAction(&producer_context->monitor_mutex, [&](){
                pthread_cond_wait(&producer_context->broker_monitor, &producer_context->monitor_mutex);
            });
        } 
        else {
            produceRequest(producer_context);
        }
    }
    return NULL;
}