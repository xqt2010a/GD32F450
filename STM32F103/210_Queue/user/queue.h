#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "stdbool.h"
#include "stdint.h"
#include "stdlib.h"

#define queue_ptf(...)  //printf(__VA_ARGS__)

typedef struct{
    uint8_t * value;
    uint32_t front;
    uint32_t rear;
    uint32_t size;
}Queue, *pQueue;

bool GreateQueue(pQueue Q, uint32_t size);
void TraverseQueue(pQueue Q);
bool FullQueue(pQueue Q);
bool EmptyQueue(pQueue Q);
bool EnQueue(pQueue Q, uint8_t val);
bool DeQueue(pQueue Q, uint8_t *val);

#endif  /* __QUEUE_H__ */