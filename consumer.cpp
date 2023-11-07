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

const int getTotalRequestsConsumed(unsigned int** requests_consumed)
{
    return requests_consumed[0][0] + requests_consumed[0][1] + requests_consumed[1][0] + requests_consumed[1][1];
}

const bool consumeRequest(Consumer* consumer_context)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(consumer_context->request_delay));
    Threading::safeAction(&consumer_context->broker_mutex, [&](){
        consumer_context->broker.pop();
    });
    pthread_cond_signal(&consumer_context->broker_monitor);
    consumer_context->requests_consumed[consumer_context->request_type][consumer_context->ledger]++;
    report_request_removed(consumer_context->ledger, consumer_context->request_type, consumer_context->requests_consumed[consumer_context->ledger], SharedData::getQueueData(consumer_context->broker));
    return getTotalRequestsConsumed(consumer_context->requests_consumed) == consumer_context->max_requests;
}

// Does something
void* ConsumerThread::consume(void* arg)
{
    auto consumer_context = (Consumer*)arg;
    bool barrier_triggered = false;
    while (!barrier_triggered)
    {
        if (consumer_context->broker.empty()) {   
            Threading::safeAction(&consumer_context->monitor_mutex, [&](){
                pthread_cond_wait(&consumer_context->broker_monitor, &consumer_context->monitor_mutex);
            });
        } else {
            barrier_triggered = consumeRequest(consumer_context);
        }
    }
    sem_post(&consumer_context->barrier);
    return NULL;
}
