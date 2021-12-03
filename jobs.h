#ifndef __JOBS__H__
#define  __JOBS__H__
    #include "commands.h"
    typedef struct {
        int id; int pid; time_t joined_time; bool running; char *name;
    } job_t;
    int jobs_refresh(void *jobs);
    int jobs_add(void *jobs, int pid, char const* name);
    int jobs_print_all(void *jobs);
    job_t jobs_get_id(void *jobs, int id);
    job_t jobs_get_last(void *jobs, int only_stopped);
    void* jobs_create();
    int jobs_remove(void* jobs, int id);
#endif
