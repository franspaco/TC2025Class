

#include "ts_queue.h"

create_queue(){
    ts_queue_t* temp = malloc(sizeof(ts_queue_t));
    ts_queue_node_t* temp_node = malloc(sizeof(ts_queue_node_t));
    temp_node->next = NULL;
    temp->root = temp_node;
    temp->tail = temp_node;
    pthread_mutex_init(&(temp->root_lock), NULL);
    pthread_mutex_init(&(temp->tail_lock), NULL);
    return temp;
}

void enqueue(ts_queue_t* queue, TYPE new_data){
    ts_queue_node_t* temp_node = malloc(sizeof(ts_queue_node_t));
    temp_node->data = new_data;
    temp_node->next = NULL;
    pthread_mutex_lock(&(queue->tail_lock));
    queue->tail->next = temp_node;
    queue->tail = temp_node;
    pthread_mutex_unlock(&(queue->tail_lock));
}

TYPE dequeue(ts_queue_t* queue){
    pthread_mutex_lock(&(queue->root_lock));
    
    pthread_mutex_unlock(&(queue->root_lock));
}