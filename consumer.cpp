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

void consumeRequest(ConsumerData* consumerData)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(consumerData->request_delay));
    Threading::safeAction(&consumerData->brokerData.broker_mutex, [&](){
        consumerData->brokerData.broker.pop();
    });
    pthread_cond_signal(&consumerData->brokerData.broker_monitor);
    // report_request_removed(consumerData->ledger, consumerData->request_type);
}

// Does something
void* Consumer::consume(void* arg)
{
    auto consumerData = (ConsumerData*)arg;
    while (1) // TODO: Add Barrier to signal quit
    {
        if (consumerData->brokerData.broker.empty())
        {
            pthread_cond_wait(&consumerData->brokerData.broker_monitor, &consumerData->brokerData.broker_mutex);
        }
        else
        {
            consumeRequest(consumerData);
        }
    }                               
}
