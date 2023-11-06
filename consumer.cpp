/* 
 * @brief 
 *
 * 
 * 
 * 
 */

#include "consumer.h"
#include "broker.h"
#include "tradecrypto.h"
#include "threading.h"

// Does something
void* Consumer::consume(void* arg)
{
    auto consumerData = (ConsumerData*)arg;
    if (consumerData->brokerData.broker.empty())
    {
        pthread_cond_wait(&consumerData->brokerData.broker_monitor, &consumerData->brokerData.broker_mutex);
    }
}
