#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "program_watcher.h"

const bool FALSE = 0;
const bool TRUE = 1;

/* Declare internal methods */
int add_runner(watched_runner_t** runners, watched_runner_t * runner);
void free_runner(watched_runner_t * runner);
watched_runner_t * get_runner(watched_runner_t ** runners, char * name);
/* END */

pid_t spawn(watched_runner_t* runner)
{
    pid_t pid = fork();

    if(pid == 0)
    {
        int length = strlen(runner->path)-7;
        char dir[length];
        strncpy(dir, runner->path, length);

        if(chdir(dir) == -1)
        {
            printf("Unable to change directory to %s\n", dir);
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
        runner->pid = pid;
    }
    return pid;
}


bool is_running(watched_runner_t * runner)
{
    int wstatus;
    int result = waitpid(runner->pid, &wstatus, 0);

    if(result == runner->pid)
    {
        printf("Watched program %s has stopped.\n", runner->program_name);
        return FALSE;
    }
    else if (result == 0){
        printf ("Watched program %s is still running.\n", runner->program_name);
        return TRUE;
    }

    return FALSE;
}


void update_watched_runners (watched_runner_t** runners, char ** directories)
{
    int index = 0;
    watched_runner_t * runner;
    int struct_size = sizeof(struct watched_runner);
    while (directories[index] != NULL)
    {
        char * program_name = strdup(strrchr(directories[index], '/')+sizeof(char));
        char * path = strdup(directories[index]);
        path = realloc(path,sizeof(char)*(strlen(path) + 7));
        path = strcat(path, "/run.sh");

        runner = malloc(struct_size);
        runner->program_name = strdup(program_name);
        runner->path = path;
        runner->pid = 0;
        runner->restart_count = 0;

        if(add_runner(runners, runner) != 0)
        {
            free_runner(runner);
            runner = get_runner(runners, program_name);
        }
        if(!is_running(runner))
            spawn(runner);

        index++;
    }
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

void free_runner(watched_runner_t * runner)
{
    free(runner->program_name);
    free(runner->path);
    free(runner);
}

int add_runner(watched_runner_t** runners, watched_runner_t * runner)
{
    int index = 0;
    // if(runners == NULL)
    //     runners = malloc(sizeof(watched_runner_t*)*0);
    /* Check if exists */
    while(runners != NULL && runners[index] != NULL)
    {
        if(strcmp(runners[index]->program_name, runner->program_name) == 0)
        {
            return -1;
        }
        index++;
    }

    runners = realloc(runners, sizeof(watched_runner_t*)*(index+1));
    printf("Adding runner for %s\n", runner->program_name);
    runners[index] = runner;
    runners[index+1] = NULL;
    return 0;
}