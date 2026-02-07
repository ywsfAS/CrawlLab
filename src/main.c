#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdatomic.h>
#include "../include/thread_pool.h"
#include "../include/crawler.h"
#include "../include/visited_set.h"
#include "../include/stats.h"
#include "../include/config.h"

crawler_stats_t stats;
atomic_int thread_job_count[WORKERS];
int main()
{
    printf("===== Crawler Test Start =====\n");

    thread_pool_t pool;
    
    thread_pool_init(&pool, WORKERS, QUEUE_CAP);
    stats_init(&stats);
    crawl_arg_t *arg = malloc(sizeof(crawl_arg_t));
    if (!arg)
    {
        perror("malloc");
        return 1;
    }

    arg->worker = &pool.worker[0];  // start from worker 0
    arg->url = strdup(URL_SEED);
    arg->depth = 0;
    
    if (!arg->url)
    {
        perror("strdup");
        return 1;
    }

    job_t job = {
        .func = crawl_job,
        .args = arg
    };
    submit_job(&pool.worker[2], job);

    printf("Seed URL submitted.\n");

    sleep(TEST_RUNTIME_SEC);

    printf("---------------------> Stopping crawler...\n");
    thread_pool_destroy(&pool);

    printf("===== Crawler Test End =====\n");
    print_crawler_stats(stats);
    printf("========= Thread Job Counts =========\n");
    for (int i = 0; i < WORKERS; i++) {
        printf("Thread %d executed %d tasks\n", i, atomic_load(&thread_job_count[i]));
    }

    save_stats_csv(stats);
    stats_destroy(&stats);
    return 0;
}

