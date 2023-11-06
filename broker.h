#ifndef BROKER_H
#define BROKER_H
#include <queue>

constexpr const int MAX_CRYPTO_REQUESTS = 15;
constexpr const int MAX_BITCOIN_REQUESTS = 6;

void* broker(void* arg);

#endif
