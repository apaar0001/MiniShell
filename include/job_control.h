#ifndef JOB_CONTROL_H
#define JOB_CONTROL_H

#include <sys/types.h>

// Job structure
typedef struct
{
    pid_t pid;
    char *command;
    int job_id;
    int state; // 0: running, 1: stopped
} Job;

// Job control functions
void init_job_control(void);
void cleanup_job_control(void);
void add_job(pid_t pid, const char *command);
void remove_job(pid_t pid);
void list_jobs(void);
void bring_to_foreground(int job_id);
void continue_background(int job_id);

#endif // JOB_CONTROL_H