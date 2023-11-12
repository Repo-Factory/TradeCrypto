/*******************************************************************************
 * @author Conner Sommerfield 824769654
 * @name tradecrypto
 * 
 * @brief Multi-thread program showing producer and consumer functionality using cryptocurrency requests.
 * 
 * @description This program has four threads, two producers and two consumers.
 * One producer is for bitcoin coins and the other is for ethereum coins. The user can use 
 * terminal arguments to control request speeds and their consumption
 * 
 * Components
 * -----------
 * Parent Thread - Define the structures of data that we are going to pass to the child threads. 
 *               - We will also define the functions to start the producer and consumer threads
 *               - Look into header file for details on the structures passed to children threads.
 * Producer - Define thread function that will add requests to the queue according to the size limits
 * Consumer - Define thread function that will remove requests from the queue when available
 * 
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

    /* ***** DIRECT NEEDED SHARED DATA TO EACH THREAD **** */
    const Producer bitcoinProducer  {broker, brokerMutex, generalMonitor, bitcoinMonitor, requestsProduced, requestsTracker, Requests::Bitcoin, args.n_flag, args.b_flag};
    const Producer ethereumProducer {broker, brokerMutex, generalMonitor, bitcoinMonitor, requestsProduced, requestsTracker, Requests::Ethereum, args.n_flag, args.e_flag};
    const Consumer xConsumer        {broker, brokerMutex, generalMonitor, bitcoinMonitor, barrier, requestsConsumed, Consumers::BlockchainX, args.n_flag, args.x_flag, barrier_triggered};
    const Consumer yConsumer        {broker, brokerMutex, generalMonitor, bitcoinMonitor, barrier, requestsConsumed, Consumers::BlockchainY, args.n_flag, args.y_flag, barrier_triggered};

    /* ***** TIE FUNCTIONS TO EACH THREAD **** */
    const ThreadContext bitcoinProducerThread  {&ProducerThread::produce, (void*)&bitcoinProducer};
    const ThreadContext ethereumProducerThread {&ProducerThread::produce, (void*)&ethereumProducer};
    const ThreadContext xConsumerThread        {&ConsumerThread::consume, (void*)&xConsumer};
    const ThreadContext yConsumerThread        {&ConsumerThread::consume, (void*)&yConsumer};

    /* ***** AGGREGATE THREAD DATA IN A SINGLE ARRAY **** */
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
