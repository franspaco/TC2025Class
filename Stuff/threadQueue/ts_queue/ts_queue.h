
#ifndef TS_QUEUE_H
#define TS_QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct ts_queue_node {
    //change type
    int* data;
    ts_queue_node_t* next;
} ts_queue_node_t;

typedef struct ts_queue {
    ts_queue_node_t* root;
    
    ts_queue_node_t* tail;
} ts_queue_t;

#endif