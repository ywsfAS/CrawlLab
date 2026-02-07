#include "../include/crawler.h"
#include "../include/stats.h"
#include "../include/normalizer.h"
#include<pthread.h>
#include<stdio.h>
void crawl_job(void *arg)
{

    crawl_arg_t *data = arg;
    if(data->depth > MAX_DEPTH) {
        free(data->url);
        free(data);
        return;
    
    }
    worker_ctx_t *worker = data->worker;
    char *url = data->url;
    char *html = fetch_page(url);
    if(!html){
        stats_inc(&stats.pages_failed , &stats.lock);
        free(url);
        free(data);
        return;
    }
    // save html
    save_to_file(HTML_DIR,url,html);
    stats_inc(&stats.urls_visited , &stats.lock);
    stats_inc(&stats.pages_success , &stats.lock);
    char * domain = extract_base(url);
    url_list_t disallow;
    url_list_init(&disallow,200);
    if(visited_check_and_add(worker->global_pool->domain,domain)){
        // fetch robots.txt

        stats_inc(&stats.domains_seen, &stats.lock);
        char new_url[200];
        snprintf(new_url,sizeof(new_url),"%s/robots.txt",domain);
        char * robots = fetch_page(new_url);
        if(robots){
            stats_inc(&stats.robots_fetched , &stats.lock);
            parse_robots(&disallow,robots);
            save_to_file(ROBOTS_DIR,domain,robots);
            free(robots);
        }else{
            printf("No robots.txt to fetch !\n");
        }

    }
    url_list_t links;
    url_list_init(&links,200);
    parse_links(&links,html);
    for (int i = 0; i < links.count; i++)
    {
        if (links.url[i] == NULL || strlen(links.url[i]) == 0 || links.url[i][0] == '#')
            continue;

        char * normalized_url = normalize_url(url,links.url[i]);
        if(is_disallowed(&disallow ,normalized_url)){
            printf("NOT ALLOWED");
            stats_inc(&stats.links_disallowed , &stats.lock);
            free(normalized_url);
            continue;
        }
        if(visited_check_and_add(worker->global_pool->visited , normalized_url)){
            crawl_arg_t *new_arg = malloc(sizeof(crawl_arg_t));
            new_arg->worker = worker;
            new_arg->url = normalized_url;
            new_arg->depth = data->depth + 1;
            printf("[Link] : %s\n",normalized_url);
            job_t new_job = {
            .func = crawl_job,
            .args = new_arg,
            };
            submit_job(worker, new_job);
        }
        else{
            stats_inc(&stats.links_duplicates , &stats.lock);
            free(normalized_url);
        }
    }
    url_list_destroy(&links);
    url_list_destroy(&disallow);
    free(url);
    free(data);
    free(html);
}

