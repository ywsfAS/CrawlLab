#ifndef STATS_H
#define STATS_HI
#include <pthread.h>
#include <stdatomic.h>
typedef struct {
    long urls_discovered;
    long urls_visited;
    long domains_seen;

    long pages_success;
    long pages_failed;
    long robots_fetched;

    long links_disallowed;
    long links_duplicates;

    long jobs_stolen;
    long jobs_submitted;
    long jobs_completed;
    pthread_mutex_t lock;

} crawler_stats_t;

extern crawler_stats_t stats;
extern atomic_int thread_job_count[8];
void stats_init(crawler_stats_t * stats);
void stats_destroy(crawler_stats_t * stats);
void stats_inc(long * value , pthread_mutex_t * lock);
void print_crawler_stats(crawler_stats_t stats);
void save_stats_csv(crawler_stats_t stats);
void sanitize_filename(char * filename);
int save_to_file(const char * dir , const char * filename ,const char * content);
#endif
