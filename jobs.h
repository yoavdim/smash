#ifndef __JOBS__H__
#define  __JOBS__H__
    #include "commands.h"
    typedef struct {
        int id; int pid; time_t joined_time; bool running; char *name;
    } job_t;
    int jobs_refresh(void *jobs);
    int jobs_add(void *jobs, int pid, char* name);
    int jobs_print_all(void *jobs);
    job_t jobs_get_id(void *jobs, int id);
#endif
