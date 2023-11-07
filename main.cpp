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
#include "semaphore.h"

int main(int argc, char* argv[])
{
    const Args args = ArgsHandling::processArgs(argc, argv);    // Handle input

    /* ***** INIT SHARED DATA **** */
    std::queue<Requests>                                         broker;
    pthread_mutex_t bitcoinMutex                                 = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t ethereumMutex                                = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t bitcoinMonitor                                = PTHREAD_COND_INITIALIZER;
    pthread_cond_t ethereumMonitor                               = PTHREAD_COND_INITIALIZER;
    unsigned int requestsProduced[RequestTypeN]                  = {0}; 
    unsigned int** requestsConsumed                              = new unsigned int*[RequestTypeN];
    requestsConsumed[0]                                          = new unsigned int[ConsumerTypeN];
    requestsConsumed[1]                                          = new unsigned int[ConsumerTypeN];
    unsigned int requestsTracker                                 = 0;

    /* ***** DIRECT NEEDED SHARED DATA TO EACH THREAD **** */
    const Producer bitcoinProducer  {broker, bitcoinMutex, bitcoinMonitor, requestsProduced, requestsTracker, Requests::Bitcoin, args.n_flag, args.b_flag};
    const Producer ethereumProducer {broker, ethereumMutex, ethereumMonitor, requestsProduced, requestsTracker, Requests::Ethereum, args.n_flag, args.e_flag};
    const Consumer bitcoinConsumer  {broker, bitcoinMutex, bitcoinMonitor, requestsConsumed, Requests::Bitcoin, Consumers::BlockchainX, args.x_flag};
    const Consumer ethereumConsumer {broker, ethereumMutex, ethereumMonitor, requestsConsumed,  Requests::Ethereum, Consumers::BlockchainY, args.y_flag};

    /* ***** TIE FUNCTIONS TO EACH THREAD **** */
    const ThreadData bitcoinProducerThread  {&ProducerThread::produce, (void*)&bitcoinProducer};
    const ThreadData ethereumProducerThread {&ProducerThread::produce, (void*)&ethereumProducer};
    const ThreadData bitcoinConsumerThread  {&ConsumerThread::consume, (void*)&bitcoinConsumer};
    const ThreadData ethereumConsumerThread {&ConsumerThread::consume, (void*)&ethereumConsumer};
    
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
    report_production_history(requestsProduced, requestsConsumed);
    return ParentThread::cleanWorkerThreads(childThreads, NUM_CHILD_THREADS);    
}
/* sem_t barrier;
    sem_init(&barrier, NULL, NULL);
    sem_wait(&barrier);
    sem_post(&barrier);  */