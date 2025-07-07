#include "../include/job_control.h"
#include "../utils/string_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_JOBS 100

static Job jobs[MAX_JOBS];
static int job_count = 0;

void init_job_control(void)
{
    job_count = 0;
    memset(jobs, 0, sizeof(jobs));
}

void cleanup_job_control(void)
{
    for (int i = 0; i < job_count; i++)
    {
        free(jobs[i].command);
    }
    job_count = 0;
    memset(jobs, 0, sizeof(jobs));
}

void add_job(pid_t pid, const char *command)
{
    if (job_count >= MAX_JOBS)
        return;

    jobs[job_count].pid = pid;
    jobs[job_count].command = my_strdup(command);
    jobs[job_count].job_id = job_count + 1;
    jobs[job_count].state = 0;
    job_count++;
}

void remove_job(pid_t pid)
{
    for (int i = 0; i < job_count; i++)
    {
        if (jobs[i].pid == pid)
        {
            free(jobs[i].command);
            for (int j = i; j < job_count - 1; j++)
            {
                jobs[j] = jobs[j + 1];
            }
            job_count--;
            return;
        }
    }
}

void list_jobs(void)
{
    for (int i = 0; i < job_count; i++)
    {
        printf("[%d] %d %s %s\n",
               jobs[i].job_id,
               jobs[i].pid,
               jobs[i].state == 0 ? "Running" : "Stopped",
               jobs[i].command);
    }
}

void bring_to_foreground(int job_id)
{
    for (int i = 0; i < job_count; i++)
    {
        if (jobs[i].job_id == job_id)
        {
            if (jobs[i].state == 1)
            {
                kill(jobs[i].pid, SIGCONT);
            }
            waitpid(jobs[i].pid, NULL, WUNTRACED);
            return;
        }
    }
}

void continue_background(int job_id)
{
    for (int i = 0; i < job_count; i++)
    {
        if (jobs[i].job_id == job_id)
        {
            if (jobs[i].state == 1)
            {
                kill(jobs[i].pid, SIGCONT);
                jobs[i].state = 0;
            }
            return;
        }
    }
}