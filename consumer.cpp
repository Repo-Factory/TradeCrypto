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

void consumeRequest(Consumer* consumerData)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(consumerData->request_delay));
    Threading::safeAction(&consumerData->broker_mutex, [&](){
        consumerData->broker.pop();
    });
    consumerData->requests_consumed[consumerData->request_type][consumerData->ledger]++;
    pthread_cond_signal(&consumerData->broker_monitor);
    report_request_removed(consumerData->ledger, consumerData->request_type, consumerData->requests_consumed[consumerData->ledger], getQueueData(consumerData->broker));
}

// Does something
void* ConsumerThread::consume(void* arg)
{
    auto consumerData = (Consumer*)arg;
    while (1) // TODO: Add Barrier to signal quit
    {
        if (consumerData->broker.empty())
        {
            pthread_cond_wait(&consumerData->broker_monitor, &consumerData->broker_mutex);
        }
        else
        {
            consumeRequest(consumerData);
        }
    }                               
}
