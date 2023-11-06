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
#include "broker.h"
#include "consumer.h"
#include "producer.h"
#include "report.h"
#include "tradecrypto.h"
#include "threading.h"

struct ProducerData
{

};

struct ConsumerData
{

};

int main(int argc, char* argv[])
{
    const Args args = ArgsHandling::processArgs(argc, argv);    // Handle input

    /* ***** INIT SHARED DATA **** */
    std::queue<int> q;

    /* ***** DIRECT NEEDED SHARED DATA TO EACH THREAD **** */
    const ProducerData producerData   {};
    const ConsumerData consumerData   {};

    /* ***** TIE FUNCTIONS TO EACH THREAD **** */
    const ThreadData producerThreadData  {&Producer::produce,                 (void*)&producerData};
    const ThreadData consumerThreadData  {&Consumer::consume,                 (void*)&consumerData};
    
    const ThreadData* threadData[NUM_CHILD_THREADS] = {&producerThreadData, &producerThreadData, &consumerThreadData, &consumerThreadData};

    /* ***** EXECUTE/MONITOR THREADS **** */
    const ThreadArray childThreads =        ParentThread::spawnWorkerThreads(threadData, NUM_CHILD_THREADS);
    return ParentThread::cleanWorkerThreads (childThreads, NUM_CHILD_THREADS);    
}
