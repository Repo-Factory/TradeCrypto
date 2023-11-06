/* 
 * @brief 
 *
 * 
 * 
 * 
 */

#include "producer.h"
#include "broker.h"
#include "tradecrypto.h"
#include "threading.h"
#include <thread>
#include <chrono>

const bool brokerFull(const std::queue<Requests>& broker)
{
    return broker.size() > MAX_CRYPTO_REQUESTS;
}

void produceRequest(const int requestDelay)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(requestDelay));
}

// Does something
void* Producer::produce(void* arg)
{
    auto producerData = (ProducerData*)arg;
    if (brokerFull(producerData->brokerData.broker))
    {
        pthread_cond_wait(&producerData->brokerData.broker_monitor, &producerData->brokerData.broker_mutex);
    }
}