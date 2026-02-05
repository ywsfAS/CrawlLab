#ifndef JOB_H
#define JOB_H

typedef void *(*task_t)(void *);

typedef struct {
    task_t func;
    void *args;
} job_t;
#endif
