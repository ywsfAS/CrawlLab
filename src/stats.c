#include "../include/stats.h"
#include <stdio.h>

void stats_inc(long *value,pthread_mutex_t *lock){
    pthread_mutex_lock(lock);
    (*value)++;
     pthread_mutex_unlock(lock);
};

void stats_init(crawler_stats_t *stats){
    memset(stats , 0 , sizeof(*stats));
    pthread_mutex_init(&stats->lock,NULL);
}
void stats_destroy(crawler_stats_t *stats){
    memset(stats , 0 , sizeof(*stats));
    pthread_mutex_destroy(&stats->lock);


}
void print_crawler_stats(crawler_stats_t stats){
   pthread_mutex_lock(&stats.lock);
 
    printf("\n");
    printf("========== CRAWLER STATISTICS ==========\n");
 
    printf("URLs Discovered      : %ld\n", stats.urls_discovered);
    printf("URLs Visited         : %ld\n", stats.urls_visited);
    printf("Domains Seen         : %ld\n", stats.domains_seen);
 
    printf("----------------------------------------\n");
 
    printf("Pages Fetched OK     : %ld\n", stats.pages_success);
    printf("Pages Failed         : %ld\n", stats.pages_failed);
    printf("Robots.txt Fetched   : %ld\n", stats.robots_fetched);
 
     printf("----------------------------------------\n");
 
    printf("Links Disallowed     : %ld\n",  stats.links_disallowed);
     printf("Links Duplicates     : %ld\n", stats.links_duplicates);
 
     printf("----------------------------------------\n");
 
    printf("Jobs Submitted       : %ld\n", stats.jobs_submitted);
    printf("Jobs Completed       : %ld\n", stats.jobs_completed);
    printf("Jobs Stolen     : %ld\n", stats.jobs_stolen);
    
    
    printf("========================================\n");
 
    pthread_mutex_unlock(&stats.lock);
  }

void save_stats_csv(crawler_stats_t stats) {
    FILE *f = fopen("crawler_stats.csv", "w");
    if (!f) return;

    fprintf(f, "Metric,Value\n");
    fprintf(f, "Domain Seen,%d\n", stats.domains_seen);
    fprintf(f, "URLs Discovered,%d\n", stats.urls_discovered);
    fprintf(f, "URLs Visited,%d\n", stats.urls_visited);
    fprintf(f, "Robots.txt Fetched OK,%d\n", stats.robots_fetched);
    fprintf(f, "Pages Fetched OK,%d\n", stats.pages_success);
    fprintf(f, "Pages Failed,%d\n", stats.pages_failed);
    fprintf(f, "Links Duplicates,%d\n", stats.links_duplicates);
    fprintf(f, "Links Disallowed,%d\n", stats.links_disallowed);
    fprintf(f, "Jobs Submitted,%d\n", stats.jobs_submitted);
    fprintf(f, "Jobs Completed,%d\n", stats.jobs_completed);
    fprintf(f, "Jobs Stolen,%d\n", stats.jobs_stolen);
    fclose(f);
}
 
