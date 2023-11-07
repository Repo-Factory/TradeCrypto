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

void produceRequest(Producer* producer_context)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(producer_context->request_delay));
    Threading::safeAction(&producer_context->broker_mutex, [&](){
        producer_context->broker.push(producer_context->request_type);
        pthread_cond_signal(&producer_context->broker_monitor);
    });
    producer_context->total_requests++;
    producer_context->requests_produced[producer_context->request_type]++;
    report_request_added(producer_context->request_type, producer_context->requests_produced, SharedData::getQueueData(producer_context->broker));
}

// Does something
void* ProducerThread::produce(void* arg)
{
    auto producer_context = (Producer*)arg;
    while (producer_context->total_requests < producer_context->max_requests)
    {
        if (producer_context->broker.size() > MAX_CRYPTO_REQUESTS)
        {
            Threading::safeAction(&producer_context->broker_mutex, [&](){
                pthread_cond_wait(&producer_context->broker_monitor, &producer_context->broker_mutex);
            });
        }
        else
        {
            produceRequest(producer_context);
        }
    }
    return NULL;
}