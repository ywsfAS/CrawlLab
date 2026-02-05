#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "../include/thread_pool.h"
#include "../include/crawler.h"

void *worker_main(void *arg);


void create_thread(pthread_t *thread, worker_ctx_t *ctx) {
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    //pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attr, STACK_SIZE_THREAD);

    if (pthread_create(thread, &attr, worker_main, ctx) != 0) {
        perror("pthread_create");
    }

    pthread_attr_destroy(&attr);
}


void create_worker(thread_pool_t *pool, worker_ctx_t *worker,
                   int id, int queue_capacity) {
    queue_init(&worker->local_queue);
    worker->id = id;
    worker->global_pool = pool;

}


bool steal_job(thread_pool_t *pool, int self_id, job_t *out) {
    for (int i = 0; i < pool->capacity; i++) {
        if (i == self_id) continue;

        worker_ctx_t *victim = &pool->worker[i];
        if(steal_queue(&victim->local_queue,out)){
            return true;
        }
    }
    return false;
}



void *worker_main(void *arg) {
    worker_ctx_t *worker = arg;
    job_t job;
    while (!worker->global_pool->shutdown) {
        // try local queue
        void * task = queue_pop(&worker->local_queue);

        if(task != NULL){
            job = *(job_t *)task; 
            test_task(arg);
            job.func(job.args);
            free(task);
            continue;
        }
        // Try stealing from other local queues
        if (steal_job(worker->global_pool, worker->id, &job)) {
            job.func(job.args);
            free(task);
            
            continue;
        }

        // Nothing to do → sleep
        sleep(1);
    }
}
void submit_job(worker_ctx_t *worker, job_t job)
{
    job_t *j = malloc(sizeof(job_t));
    if (!j) return;  // allocation failed

    *j = job;  // copy job into heap

    queue_t *q = &worker->local_queue;

    pthread_mutex_lock(&q->lock);

    // non-blocking: just check full
    if (is_queue_full(q) || worker->global_pool->shutdown)
    {
        pthread_mutex_unlock(&q->lock);
        free(j);  // drop job
        return;
    }

    // there is space → push job
    queue_push(q, j);
    pthread_cond_signal(&q->cv);  // wake a worker

    pthread_mutex_unlock(&q->lock);
}


void thread_pool_init(thread_pool_t *pool,
                      int workers_num,
                      int queue_capacity) {
    pool->capacity = workers_num;
    pool->visited = hash_table_create(4000,hash);
    pool->shutdown = false;
    pool->worker = malloc(sizeof(worker_ctx_t) * workers_num);

    for (int i = 0; i < workers_num; i++) {
        create_worker(pool, &pool->worker[i], i, queue_capacity);
    }
    for (int i = 0; i < workers_num; i++) {
        create_thread(&pool->worker[i].thread , &pool->worker[i]);
    }
}
void thread_pool_destroy(thread_pool_t * pool){
    pool->shutdown = true;
    for (int i = 0; i < pool->capacity; i++) {

        pthread_mutex_lock(&pool->worker[i].local_queue.lock);

        pthread_cond_broadcast(&pool->worker[i].local_queue.cv);

        pthread_mutex_unlock(&pool->worker[i].local_queue.lock);
    }
    for(int i = 0 ; i < pool->capacity ; i++){
        pthread_join(pool->worker[i].thread,NULL);
    }
    hash_table_destroy(pool->visited);
    for(int i = 0 ; i < pool->capacity ; i++){
        queue_destroy(&pool->worker[i].local_queue);
    }
    free(pool->worker);


}

void *test_task(void *arg) {
    int id = *(int *)arg;
    printf("Task %d executed by thread run func %lu\n", id, pthread_self());
    return NULL;
}


