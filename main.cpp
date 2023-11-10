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
    pthread_mutex_t bitcoinMutex                                 = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t ethereumMutex                                = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t bitcoinMonitor                                = PTHREAD_COND_INITIALIZER;
    pthread_cond_t ethereumMonitor                               = PTHREAD_COND_INITIALIZER;
    unsigned int* requestsProduced                               = new unsigned int [RequestTypeN];
    unsigned int** requestsConsumed                              = new unsigned int*[RequestTypeN];
    requestsConsumed[Requests::Bitcoin]                          = new unsigned int [ConsumerTypeN];
    requestsConsumed[Requests::Ethereum]                         = new unsigned int [ConsumerTypeN];
    unsigned int requestsTracker                                 = INIT_INT;
    sem_t                                                        barrier;
    sem_init                                                     (&barrier, 0, 0);
    bool                                                         barrier_triggered;

    /* ***** DIRECT NEEDED SHARED DATA TO EACH THREAD **** */
    const Producer bitcoinProducer  {broker, brokerMutex, bitcoinMutex, bitcoinMonitor, requestsProduced, requestsTracker, Requests::Bitcoin, args.n_flag, args.b_flag};
    const Producer ethereumProducer {broker, brokerMutex, ethereumMutex, ethereumMonitor, requestsProduced, requestsTracker, Requests::Ethereum, args.n_flag, args.e_flag};
    const Consumer bitcoinConsumer  {broker, brokerMutex, bitcoinMutex, bitcoinMonitor, barrier, requestsConsumed, Consumers::BlockchainX, args.n_flag, args.x_flag, barrier_triggered};
    const Consumer ethereumConsumer {broker, brokerMutex, ethereumMutex, ethereumMonitor, barrier, requestsConsumed, Consumers::BlockchainY, args.n_flag, args.y_flag, barrier_triggered};

    /* ***** TIE FUNCTIONS TO EACH THREAD **** */
    const ThreadContext bitcoinProducerThread  {&ProducerThread::produce, (void*)&bitcoinProducer};
    const ThreadContext ethereumProducerThread {&ProducerThread::produce, (void*)&ethereumProducer};
    const ThreadContext bitcoinConsumerThread  {&ConsumerThread::consume, (void*)&bitcoinConsumer};
    const ThreadContext ethereumConsumerThread {&ConsumerThread::consume, (void*)&ethereumConsumer};

    const ThreadContext* threadData[NUM_CHILD_THREADS] = 
    {
        &bitcoinProducerThread, 
        &ethereumProducerThread, 
        &bitcoinConsumerThread, 
        &ethereumConsumerThread
    }; 

    /* ***** EXECUTE/MONITOR THREADS **** */
    const ThreadArray childThreads = ParentThread::spawnWorkerThreads(threadData, NUM_CHILD_THREADS);
    sem_wait(&barrier);
    report_production_history(requestsProduced, requestsConsumed);
    return ParentThread::cleanWorkerThreads(childThreads, NUM_CHILD_THREADS);    
}
