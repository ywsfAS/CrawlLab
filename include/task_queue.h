#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H
#include<stdbool.h>
#include<pthread.h>
#include "job.h"
#define QUEUE_CAPACITY 100
typedef struct {
  void * queue[QUEUE_CAPACITY];
  int head;
  int tail;
  int capacity;
  pthread_cond_t cv;
  pthread_mutex_t lock;
}queue_t;


void  queue_init(queue_t * queue);
bool  queue_push(queue_t * queue , void * obj);
void* queue_pop(queue_t * queue);
void  queue_destroy(queue_t * queue);
bool steal_queue(queue_t * queue ,  job_t *out);
bool is_queue_full(queue_t * queue);



#endif
