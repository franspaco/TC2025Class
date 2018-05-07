
#ifndef TS_QUEUE_H
#define TS_QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define TYPE int*

typedef struct ts_queue_node {
    //change type
    TYPE data;
    ts_queue_node_t* next;
} ts_queue_node_t;

typedef struct ts_queue {
    ts_queue_node_t* root;
    pthread_mutex_t root_lock;
    ts_queue_node_t* tail;
    pthread_mutex_t tail_lock;
} ts_queue_t;

ts_queue_t* create_queue();

void enqueue(ts_queue_t* queue, TYPE new_data);

TYPE dequeue(ts_queue_t* queue);

#endif