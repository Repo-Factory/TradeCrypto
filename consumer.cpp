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
#include <thread>
#include <chrono>

void consumeRequest(const int requestDelay)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(requestDelay));
}

// Does something
void* Consumer::consume(void* arg)
{
    auto consumerData = (ConsumerData*)arg;
    if (consumerData->brokerData.broker.empty())
    {
        pthread_cond_wait(&consumerData->brokerData.broker_monitor, &consumerData->brokerData.broker_mutex);
    }
}
