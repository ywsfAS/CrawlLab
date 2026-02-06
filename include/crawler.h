#ifndef CRAWLER_H
#define CRAWLER_H
#include "thread_pool.h"
#include "fetcher.h"
#include "parser.h"
#include "visited_set.h"
#define MAX_DEPTH 5
typedef struct {
    worker_ctx_t *worker;
    char *url;
    int depth;
}crawl_arg_t;
void crawl_job(void * arg);


#endif
