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

const bool QUEUE_EMPTY(Consumer* consumer_context)
{
    return consumer_context->broker.empty();
}

const bool consumeRequest(Consumer* consumer_context)
{
    const RequestType type = consumer_context->broker.front();
    consumer_context->broker.pop();
    consumer_context->requests_consumed[consumer_context->ledger][type]++;
    report_request_removed(consumer_context->ledger, type, consumer_context->requests_consumed[consumer_context->ledger], SharedData::getQueueData(consumer_context->broker).data());
    if (type == Requests::Bitcoin) pthread_cond_signal(&consumer_context->bitcoin_monitor);
    pthread_cond_signal(&consumer_context->general_monitor);
    return getTotalRequestsConsumed(consumer_context->requests_consumed) == consumer_context->max_requests;
}

// Does something
void* ConsumerThread::consume(void* arg)
{
    auto consumer_context = (Consumer*)arg;
    while (!consumer_context->barrier_triggered)
    {
        pthread_mutex_lock(&consumer_context->broker_mutex);
        while (consumer_context->broker.empty()) {   
            pthread_cond_wait(&consumer_context->general_monitor, &consumer_context->broker_mutex);
        } 
        consumer_context->barrier_triggered = consumeRequest(consumer_context);
        pthread_mutex_unlock(&consumer_context->broker_mutex);
        std::this_thread::sleep_for(std::chrono::milliseconds(consumer_context->request_delay));
    }
    sem_post(&consumer_context->barrier);
    return NULL;
}
