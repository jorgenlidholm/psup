#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include "program_watcher.h"

const bool FALSE = 0;
const bool TRUE = 1;

#define E_EXISTS -1
#define E_RESTART -2

/* Declare internal methods */
pid_t spawn(watched_runner_t * runner);
bool is_running(watched_runner_t * runner);
watched_runner_t** add_runner(watched_runner_t** runners, watched_runner_t * runner, int *response);
void free_runner(watched_runner_t * runner);
watched_runner_t * get_runner(watched_runner_t ** runners, char * name);
time_t get_file_time(char * filepath);
/* END */
char dir[4096];
const char * dirptr = dir;

pid_t spawn(watched_runner_t* runner)
{

    /* Child process. */
    int length = strlen(runner->path)-7;
    strncpy(dir, runner->path, length+1);
    
    pid_t pid = fork();

    if(pid == 0)
    {
        /* Child process. */

        if(chdir(dirptr) == -1)
        {
            // printf("Unable to change directory to %s\n", dir);
            exit (-1);
        }
        char * arg[] = { runner->path, "start", NULL};
        int exec_result = execvp(runner->path, arg);
        if(exec_result != 0)
        {
            perror("execvp");
            return -1;
        }
        exit (1);
    }
    else{
        runner->file_time = get_file_time(runner->path);
        runner->pid = pid;
    }
    
    return pid;
}


void stop(watched_runner_t* runner)
{
    int length = strlen(runner->path)-7;
    char * dir = calloc(sizeof(char), length+2);
    strncpy(dir, runner->path, length);
    dir[length+1] = '\0';
    if(chdir(dir) == -1)
    {
        free(dir);
        return;
    }
    free(dir);

    char * cmd = strdup(runner->path);
    cmd = realloc(cmd, strlen(cmd) + 6);
    cmd = strcat(cmd, " stop");
    if(system(cmd) < 0){
        perror("Failed to execute");
    }
    free(cmd);
}

void stop_all(watched_runner_t ** runners)
{
    int index=0;
    while(runners != NULL && runners[index] != NULL)
    {
        stop(runners[index]);
        index++;
    }
}

bool is_running(watched_runner_t * runner)
{
    int wstatus;
    int result;

    if(runner->pid == 0)
        return FALSE;

    result = waitpid(runner->pid, &wstatus, WNOHANG);
    if (result == 0){
        return TRUE;
    }

    if(result == runner->pid)
    {
        printf("Watched program %s has stopped.\n", runner->program_name);
        return FALSE;
    }

    return FALSE;
}


watched_runner_t** update_watched_runners (watched_runner_t** runners, char ** directories)
{
    int index = 0;
    watched_runner_t * runner;
    int struct_size = sizeof(struct watched_runner);
    while (directories[index] != NULL)
    {
        char * program_name = strdup(strrchr(directories[index], '/')+sizeof(char));
        char * path = strdup(directories[index]);
        path = realloc(path,sizeof(char)*(strlen(path) + 8));
        path = strcat(path, "/run.sh");

        runner = malloc(struct_size);
        runner->program_name = strdup(program_name);
        runner->path = path;
        runner->pid = 0;
        runner->restart_count = 0;
        runner->file_time = get_file_time(path);

        int response = 0;
        runners = add_runner(runners, runner, &response);
        if(response != 0)
        {
            switch(response)
            {
                case E_EXISTS:
                    free_runner(runner);
                    runner = get_runner(runners, program_name);
                    break;
                case E_RESTART:
                    printf("Files updated, sleeping...\n");
                    sleep(3);
                    printf("done sleeping, stopping %s to restart.\n", program_name);
                    runner = get_runner(runners, program_name);
                    stop(runner);
                    break;
            }
        }
        if(!is_running(runner))
            spawn(runner);

        index++;
    }

    return runners;
}

time_t get_file_time(char * filepath)
{
    struct stat attrib;
    stat(filepath, &attrib);
    return attrib.st_mtime;
}

watched_runner_t * get_runner(watched_runner_t ** runners, char * name)
{
    int index = 0;
    while(runners != NULL || runners[index] != NULL)
    {
        if(strcmp(runners[index]->program_name, name) == 0)
        {
            return runners[index];
        }
        index++;
    }
    return NULL;
}

void free_all_runners(watched_runner_t** runners)
{
    int index = 0;
    while(runners!= NULL && runners[index] != NULL)
    {
        free_runner(runners[index]);
        index++;
    }
}
void free_runner(watched_runner_t * runner)
{
    free(runner->program_name);
    free(runner->path);
    free(runner);
}

watched_runner_t** add_runner(watched_runner_t** runners, watched_runner_t * runner, int * response)
{
    int index = 0;
    /* Check if exists */
    while(runners != NULL && runners[index] != NULL)
    {
        if(strcmp(runners[index]->program_name, runner->program_name) == 0)
        {
            if (runners[index]->file_time < runner->file_time)
            {
                *response = E_RESTART;
                return runners;
            }
            *response = E_EXISTS;
            return runners;
        }
        index++;
    }

    watched_runner_t ** p = realloc(runners, sizeof(watched_runner_t*)*(index+2));
    if(p == NULL)
        perror("reallocation failed.");
    runners = p;
    printf("Adding runner for %s\n", runner->program_name);
    runners[index] = runner;
    runners[index+1] = NULL;
    return runners;
}