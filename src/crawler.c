#include "../include/crawler.h"
#include<pthread.h>
#include<stdio.h>
void crawl_job(void *arg)
{
    crawl_arg_t *data = arg;
    worker_ctx_t *worker = data->worker;
    char *url = data->url;
    char *html = fetch_page(url);
    if(!html){
        free(url);
        free(data);
        return;
    }
    
    url_list_t links;
    url_list_init(&links,200);
    parse_links(&links,html);
    printf("------->[Thread %lu] Crawling %s\n",
    pthread_self(), url);
    for (int i = 0; i < links.count; i++)
    {
        if (links.url[i] == NULL || strlen(links.url[i]) == 0 || links.url[i][0] == '#')
            continue;
        if(visited_check_and_add(worker->global_pool->visited , links.url[i])){
            crawl_arg_t *new_arg = malloc(sizeof(crawl_arg_t));
            new_arg->worker = worker;
            new_arg->url = strdup(links.url[i]);
            printf("[Link] : %s\n",links.url[i]);
            job_t new_job = {
            .func = crawl_job,
            .args = new_arg
            };
            submit_job(worker, new_job);
        }
    }
    url_list_destroy(&links);
    free(url);
    free(data);
    free(html);
}

