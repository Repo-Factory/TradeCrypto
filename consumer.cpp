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

unsigned int* getQueueData(std::queue<Requests> q)
{
    const size_t queueSize = q.size();
    unsigned int* dataArray = new unsigned int[queueSize];
    for (size_t i = 0; i < queueSize; i++) {
        dataArray[i] = q.front(); q.pop();
    }
    return dataArray;
}

void consumeRequest(ConsumerData* consumerData)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(consumerData->request_delay));
    Threading::safeAction(&consumerData->broker_data.broker_mutex, [&](){
        consumerData->broker_data.broker.pop();
    });
    consumerData->requests_consumed[consumerData->request_type][consumerData->ledger]++;
    pthread_cond_signal(&consumerData->broker_monitor);
    report_request_removed(consumerData->ledger, consumerData->request_type, consumerData->requests_consumed[consumerData->ledger], getQueueData(consumerData->broker_data.broker));
}

// Does something
void* Consumer::consume(void* arg)
{
    auto consumerData = (ConsumerData*)arg;
    while (1) // TODO: Add Barrier to signal quit
    {
        if (consumerData->broker_data.broker.empty())
        {
            pthread_cond_wait(&consumerData->broker_monitor, &consumerData->broker_data.broker_mutex);
        }
        else
        {
            consumeRequest(consumerData);
        }
    }                               
}
