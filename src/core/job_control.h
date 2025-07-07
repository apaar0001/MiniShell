#ifndef JOB_CONTROL_H
#define JOB_CONTROL_H

#include <sys/types.h>

#define MAX_JOBS 100

typedef struct
{
    pid_t pid;
    char *command;
    int job_id;
    int state; // 0 for running, 1 for stopped
} Job;

// Job control functions
void add_job(pid_t pid, const char *command);
void remove_job(pid_t pid);
void list_jobs(void);
void bring_to_foreground(int job_id);
void continue_background(int job_id);

#endif // JOB_CONTROL_H