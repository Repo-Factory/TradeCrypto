/* 
 * @brief 
 *
 * 
 * 
 * 
 */

#include "broker.h"


/* The broker can hold up to a maximum of 15 crypto requests in its request queue at
any given time.
a. When the trade request queue is full, producers must wait for consumers
to consume a request before they can publish another crypto transaction
request to the queue.
2 | P a g e
b. When the trade request queue is empty, consumers must wait for
producers to add / insert a new request to the broker before they can
consume another request off from the queu */
