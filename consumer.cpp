/* 
 * @brief 
 *
 * 
 * 
 * 
 */

#include "consumer.h"
#include "tradecrypto.h"
#include "parent_thread.h"
#include "report.h"
#include <thread>
#include <chrono>

void consumeRequest(Consumer* consumer_context)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(consumer_context->request_delay));
    Threading::safeAction(&consumer_context->broker_mutex, [&](){
        consumer_context->broker.pop();
        pthread_cond_signal(&consumer_context->broker_monitor);
    });
    consumer_context->requests_consumed[consumer_context->request_type][consumer_context->ledger]++;
    report_request_removed(consumer_context->ledger, consumer_context->request_type, consumer_context->requests_consumed[consumer_context->ledger], getQueueData(consumer_context->broker));
}

// Does something
void* ConsumerThread::consume(void* arg)
{
    auto consumer_context = (Consumer*)arg;
    while (1) // TODO: Add Barrier to signal quit
    {
        if (consumer_context->broker.empty())
        {   Threading::safeAction(&consumer_context->broker_mutex, [&](){
                pthread_cond_wait(&consumer_context->broker_monitor, &consumer_context->broker_mutex);
            });
        }
        else
        {
            consumeRequest(consumer_context);
        }
    }                               
}
