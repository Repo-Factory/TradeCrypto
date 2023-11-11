/*******************************************************************************
 * @author Conner Sommerfield 824769654
 * @name tradecrypto
 * 
 * @brief 
 * 
 * @description 
 * 
 * Consumner
 * Producer
 * Broker
 * See cpp file of each for more details in each respective area
 * 
 ******************************************************************************/

#include "args_handling.h"
#include "consumer.h"
#include "producer.h"
#include "report.h"
#include "tradecrypto.h"
#include "parent_thread.h"
#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
    const Args args = ArgsHandling::processArgs(argc, argv);    // Handle input

    /* ***** INIT SHARED DATA **** */
    std::queue<Requests>                                         broker;
    pthread_mutex_t brokerMutex                                  = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t generalMutex                                 = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t bitcoinMutex                                 = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t generalMonitor                                = PTHREAD_COND_INITIALIZER;
    pthread_cond_t bitcoinMonitor                                = PTHREAD_COND_INITIALIZER;
    unsigned int* requestsProduced                               = new unsigned int [RequestTypeN];
    unsigned int** requestsConsumed                              = new unsigned int*[RequestTypeN];
    requestsConsumed[Requests::Bitcoin]                          = new unsigned int [ConsumerTypeN];
    requestsConsumed[Requests::Ethereum]                         = new unsigned int [ConsumerTypeN];
    unsigned int requestsTracker                                 = INIT_INT;
    sem_t                                                        barrier;
    sem_init                                                     (&barrier, 0, 0);
    bool                                                         barrier_triggered;

/* struct Producer
{
    std::queue<Requests>& broker;
    pthread_mutex_t& broker_mutex;
    pthread_mutex_t& general_mutex;
    pthread_mutex_t& bitcoin_mutex;
    pthread_cond_t& general_monitor;
    pthread_cond_t& bitcoin_monitor;
    unsigned int* requests_produced;
    unsigned int& total_requests;
    const Requests request_type;
    const int max_requests;
    const int request_delay;
};

struct Consumer
{
    std::queue<Requests>& broker;
    pthread_mutex_t& broker_mutex;
    pthread_mutex_t& general_mutex;
    pthread_mutex_t& bitcoin_mutex;
    pthread_cond_t& general_monitor;
    pthread_cond_t& bitcoin_monitor;
    sem_t& barrier;
    unsigned int** requests_consumed;
    const Consumers ledger;
    const int max_requests;
    const int request_delay;
    bool& barrier_triggered; */

    /* ***** DIRECT NEEDED SHARED DATA TO EACH THREAD **** */
    const Producer bitcoinProducer  {broker, brokerMutex, generalMutex, bitcoinMutex, generalMonitor, bitcoinMonitor, requestsProduced, requestsTracker, Requests::Bitcoin, args.n_flag, args.b_flag};
    const Producer ethereumProducer {broker, brokerMutex, generalMutex, bitcoinMutex, generalMonitor, bitcoinMonitor, requestsProduced, requestsTracker, Requests::Ethereum, args.n_flag, args.e_flag};
    const Consumer xConsumer        {broker, brokerMutex, generalMutex, bitcoinMutex, generalMonitor, bitcoinMonitor, barrier, requestsConsumed, Consumers::BlockchainX, args.n_flag, args.x_flag, barrier_triggered};
    const Consumer yConsumer        {broker, brokerMutex, generalMutex, bitcoinMutex, generalMonitor, bitcoinMonitor, barrier, requestsConsumed, Consumers::BlockchainY, args.n_flag, args.y_flag, barrier_triggered};

    /* ***** TIE FUNCTIONS TO EACH THREAD **** */
    const ThreadContext bitcoinProducerThread  {&ProducerThread::produce, (void*)&bitcoinProducer};
    const ThreadContext ethereumProducerThread {&ProducerThread::produce, (void*)&ethereumProducer};
    const ThreadContext xConsumerThread        {&ConsumerThread::consume, (void*)&xConsumer};
    const ThreadContext yConsumerThread        {&ConsumerThread::consume, (void*)&yConsumer};

    const ThreadContext* threadData[NUM_CHILD_THREADS] = 
    {
        &bitcoinProducerThread, 
        &ethereumProducerThread, 
        &xConsumerThread, 
        &yConsumerThread
    }; 

    /* ***** EXECUTE/MONITOR THREADS **** */
    const ThreadArray childThreads = ParentThread::spawnWorkerThreads(threadData, NUM_CHILD_THREADS);
    sem_wait(&barrier);
    report_production_history(requestsProduced, requestsConsumed);
    return ParentThread::cleanWorkerThreads(childThreads, NUM_CHILD_THREADS);    
}
