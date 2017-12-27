#include "queue.h"

bool GreateQueue(pQueue Q, uint32_t size)
{
    Q->value = (uint8_t *)malloc(sizeof(uint8_t *)*size);
    if(NULL == Q->value){
        return false;
    }
    Q->front = 0;
    Q->rear = 0;
    Q->size = size;
    return true;
}

void TraverseQueue(pQueue Q)
{
    uint32_t i = Q->front;
    queue_ptf("Queue data:\r\n")
    while((i % Q->size) != Q->rear){
        queue_ptf("%d ",Q->value[i])
        i++;
    }
    queue_ptf("\r\n");
}

bool FullQueue(pQueue Q)
{
    if(Q->front == ((Q->rear+1)%Q->size)){
        return true;
    }
    return false;
}

bool EmptyQueue(pQueue Q)
{
    if(Q->front == Q->rear){
        return true;
    }
    return false;
}

bool EnQueue(pQueue Q, uint8_t val)  //入列
{
    if(FullQueue(Q))
        return false;
    Q->value[Q->rear] = val;
    Q->rear = (Q->rear + 1) % Q->size;
    return true;
}

bool DeQueue(pQueue Q, uint8_t *val)  //出列
{
    if(EmptyQueue(Q))
        return false;
    *val = Q->value[Q->front];
    Q->front = (Q->front + 1)%Q->size;
    return true;
}
