#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>
#include<../include/task_queue.h>
#include<../include/crawler.h>


void queue_init(queue_t * queue){
  memset(queue, 0 , sizeof(*queue));
  queue->capacity = QUEUE_CAPACITY;
  pthread_cond_init(&queue->cv,NULL);
  pthread_mutex_init(&queue->lock,NULL);

}
bool is_queue_full(queue_t * queue ){
  if( (queue->tail + 1) % queue->capacity == queue->head){
      return true;
  }
  return false;
}
bool queue_push(queue_t * queue ,void * obj){
  if( (queue->tail + 1) % queue->capacity == queue->head){
      return false;
  }
  queue->queue[queue->tail] = obj;
  queue->tail = (queue->tail + 1) % queue->capacity; 
  return true;
}
void * queue_pop(queue_t * queue){
  pthread_mutex_lock(&queue->lock);
  if(queue->tail == queue->head){
    pthread_mutex_unlock(&queue->lock);  
    return NULL;
  }
  int head = queue->head; 
  queue->head = (head + 1) % queue->capacity;
  pthread_mutex_unlock(&queue->lock);
  return queue->queue[head];
}
void queue_destroy(queue_t *queue) {
    if (!queue) return;

    int i = queue->head;
    while (i != queue->tail) { // walk until we reach tail
        job_t *job = queue->queue[i];
        if (job) {
            crawl_arg_t *arg = (crawl_arg_t *)job->args;
            free(arg->url);
            free(arg);
            free(job);
        }
        i = (i + 1) % queue->capacity;
    }

    pthread_cond_destroy(&queue->cv);
    pthread_mutex_destroy(&queue->lock);
    memset(queue, 0, sizeof(*queue));
}



bool steal_queue(queue_t *queue, job_t *out)
{
    pthread_mutex_lock(&queue->lock);

    if (queue->head == queue->tail) {
        pthread_mutex_unlock(&queue->lock);
        return false;
    }

    int last = (queue->tail + queue->capacity - 1) % queue->capacity;

    job_t *j = (job_t *)queue->queue[last];

    *out = *j;
    free(j);

    queue->tail = last;

    pthread_mutex_unlock(&queue->lock);
    return true;
}

