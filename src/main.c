#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../include/thread_pool.h"
#include "../include/crawler.h"
#include "../include/visited_set.h"

#define WORKERS 3
#define QUEUE_CAP 640
#define TEST_RUNTIME_SEC 10   // how long crawler runs (for testing)

int main()
{
    printf("===== Crawler Test Start =====\n");

    thread_pool_t pool;

    thread_pool_init(&pool, WORKERS, QUEUE_CAP);

    crawl_arg_t *arg = malloc(sizeof(crawl_arg_t));
    if (!arg)
    {
        perror("malloc");
        return 1;
    }

    arg->worker = &pool.worker[0];  // start from worker 0
    arg->url = strdup("http://httpforever.com");
    
    if (!arg->url)
    {
        perror("strdup");
        return 1;
    }

    job_t job = {
        .func = crawl_job,
        .args = arg
    };
    submit_job(&pool.worker[0], job);

    printf("Seed URL submitted.\n");

    sleep(TEST_RUNTIME_SEC);

    printf("---------------------> Stopping crawler...\n");
    sleep(3);

    thread_pool_destroy(&pool);

    printf("===== Crawler Test End =====\n");

    return 0;
}

