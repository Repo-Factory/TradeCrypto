/*******************************************************************************
 * @author Conner Sommerfield 824769654
 * @name tradecrypto
 * 
 * @brief Perform virtual page mapping to a physical address with page replacement.
 * 
 * @description We will use a tree implementation to map virtual addresses taken from a trace file to physical addresses
 * We will also keep track of read/write accesses and store the appropriate information to provide page replacement when necessary
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
#include "semaphore.h"

int main(int argc, char* argv[])
{
    const Args args = ArgsHandling::processArgs(argc, argv);    // Handle input

    /* ***** INIT SHARED DATA **** */
    std::queue<Requests>                                         broker;
    pthread_mutex_t brokerMutex                                  = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t bitcoinMonitor                                = PTHREAD_COND_INITIALIZER;
    pthread_cond_t ethereumMonitor                               = PTHREAD_COND_INITIALIZER;
    BrokerData brokerData                                        {broker, brokerMutex};
    unsigned int requestsProduced[RequestTypeN]                  = {0}; 
    unsigned int requestsConsumed[RequestTypeN][ConsumerTypeN]   = {{0}, {0}};
    unsigned int requestsTracker                                 =  0;

    /* ***** DIRECT NEEDED SHARED DATA TO EACH THREAD **** */
    const ProducerData bitcoinProducer  {brokerData, bitcoinMonitor, requestsProduced, requestsTracker, Requests::Bitcoin, args.n_flag, args.b_flag};
    const ProducerData ethereumProducer {brokerData, ethereumMonitor, requestsProduced, requestsTracker, Requests::Ethereum, args.n_flag, args.e_flag};
    const ConsumerData bitcoinConsumer  {brokerData, bitcoinMonitor, (unsigned int**)requestsConsumed, Requests::Bitcoin, Consumers::BlockchainX, args.x_flag};
    const ConsumerData ethereumConsumer {brokerData, ethereumMonitor, (unsigned int**)requestsConsumed,  Requests::Ethereum, Consumers::BlockchainY, args.y_flag};

    /* ***** TIE FUNCTIONS TO EACH THREAD **** */
    const ThreadData bitcoinProducerThread  {&Producer::produce, (void*)&bitcoinProducer};
    const ThreadData ethereumProducerThread {&Producer::produce, (void*)&ethereumProducer};
    const ThreadData bitcoinConsumerThread  {&Consumer::consume, (void*)&bitcoinConsumer};
    const ThreadData ethereumConsumerThread {&Consumer::consume, (void*)&ethereumConsumer};
    
    const ThreadData* threadData[NUM_CHILD_THREADS] = 
    {
        &bitcoinProducerThread, 
        &ethereumProducerThread, 
        &bitcoinConsumerThread, 
        &ethereumConsumerThread
    };

    /* ***** EXECUTE/MONITOR THREADS **** */
    const ThreadArray childThreads = ParentThread::spawnWorkerThreads(threadData, NUM_CHILD_THREADS);
    for (const auto& thread : childThreads) {
        pthread_join(*thread, NULL); // Wait for threads for clean exit
    }
    report_production_history(requestsProduced, (unsigned int**)requestsConsumed);
    return ParentThread::cleanWorkerThreads(childThreads, NUM_CHILD_THREADS);    
}
/* sem_t barrier;
    sem_init(&barrier, NULL, NULL);
    sem_wait(&barrier);
    sem_post(&barrier);  */