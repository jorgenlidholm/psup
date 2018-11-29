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

/* Internal methods */
int add_runner(watched_runner_t** runners, watched_runner_t * runner);

pid_t spawn(watched_runner_t* runner)
{
    pid_t pid = fork();

    if(pid == 0)
    {
        char * arg[] = { runner->path, "start", NULL};
        int exec_result = execvp(runner->path, arg);
        if(exec_result != 0)
        {
            perror("execvp");
            return -1;
        }
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
    
    while (directories[index] != NULL)
    {
        watched_runner_t * runner = malloc(sizeof(watched_runner_t));
        runner->program_name = strdup(rindex(directories[index], '/')+sizeof(char));
        runner->path = strcat(strdup(directories[index]), "/run.sh");
        if(add_runner(runners, runner) != 0)
        {
            free(runner);
            index++;
            continue;
        }
        spawn(runner);
        index++;
    }
}

int add_runner(watched_runner_t** runners, watched_runner_t * runner)
{
    int index = 0;
    /* Check if exists */
    while(runners[index] != NULL)
    {
        if(runners[index]->program_name == runner->program_name)
        {
            return -1;
        }
        index++;
    }

    runners = realloc(runners, sizeof(watched_runner_t*)*index);
    runners[index] = runner;
    return 0;
}