// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.h"
#include "jobs.h"

void handler_cntlc(int sig) {
    int pid = fg_pid;
    if(fg_pid > 0) {
        fg_pid = 0;
        kill(pid, sig);
    }
}

void set_handlers() {
    struct sigaction act;

    act.sa_handler = &handler_cntlc;
    sigfillset(&(act.sa_mask));

    sigaction(SIGINT, &act, NULL);
    sigaction(SIGTSTP, &act, NULL);
}
