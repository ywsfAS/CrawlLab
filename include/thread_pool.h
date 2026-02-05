#ifndef THREAD_POOL_H
#define THREAD_POOL_H
#include <pthread.h>
#include <stdbool.h>
#include "task_queue.h"
#include "hash_table.h"
#include "job.h"
#define STACK_SIZE_THREAD (1024 * 1024)

typedef struct thread_pool thread_pool_t;

typedef struct {
    int id;
    pthread_t thread;
    queue_t local_queue;
    thread_pool_t *global_pool;
} worker_ctx_t;

struct thread_pool {
    worker_ctx_t *worker;
    int capacity;
    hash_table_t* visited;
    bool shutdown;
};
void create_thread(pthread_t * thread , worker_ctx_t * ctx);
void create_worker(thread_pool_t * pool , worker_ctx_t * worker, int id , int queue_capacity);
bool steal_job(thread_pool_t * pool , int self_id , job_t *out);
void *worker_main(void *arg);
void submit_job(worker_ctx_t * worker, job_t job);
void thread_pool_init(thread_pool_t * pool, int workers_num , int queue_capacity );
void thread_pool_destroy(thread_pool_t * pool);
void * test_task(void * arg);
#endif
