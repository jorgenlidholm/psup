#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <signal.h>
#define _GNU_SOURCE
#include <errno.h>

#include "directory_watcher.h"
#include "program_watcher.h"
#include "process_helper.h"

watched_runner_t ** runners;
bool is_dir(char* name);
void send_sigint_to_instance(char* directory);
void handler(int sig, siginfo_t *info, void *ucontext);

void print_usage(char * name)
{
    printf("Usage %s --directory <dir>\n", name);
}

extern char *program_invocation_name;
extern char *program_invocation_short_name;

int main(int argc, char ** argv)
{
    struct sigaction act;
    bool stop_all_watched_applications = FALSE;

    printf ("### Fleetech Supervisor ###\n\n");
    if(argc < 1)
    {
        printf("Wrong argument count.\n");
        print_usage(argv[0]);
        exit(-1);
    }

    static struct option long_options[] = {
        {"directory", required_argument, 0, 0},
        {"stop", no_argument, 0, 1 },
        {0,0,0,0}
    };

    char * directory;    
    int opt;
    int option_index = 0;
    while((opt = getopt_long_only(argc, argv, "", long_options, &option_index)) != -1)
    {
        switch(opt)
        {
            case 'd':
            case 0:
                directory = malloc(sizeof(char)*strlen(optarg)+1);
                strcpy(directory, optarg);
                break;
            case 's':
            case 1:
                stop_all_watched_applications = TRUE;
                break;
            default:
                print_usage(argv[0]);
        }
    }


    if(stop_all_watched_applications)
    {
         /* Försök hitta fsup som övervakar <directory> */
        send_sigint_to_instance(directory);
        exit(0);
    }

    if(!is_dir(directory))
    {
        printf("%s is not a valid directory, or you don't have access.\n", directory);
        print_usage(argv[0]);
        exit (-1);
    }

    /* Sätt upp hantering mot Ctrl-C (SIGINT) */
    memset (&act, '\0', sizeof(act));
    act.sa_handler = (void*)&handler;
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGHUP, &act, NULL);


    printf("Watching folder '%s'.\n", directory);

    runners = NULL;
    // malloc(sizeof(watched_runner_t**));
    // *runners = NULL;
    while (TRUE) {
        char ** response = get_directories_with_runsh(directory);
        runners = update_watched_runners(runners, response);
        free_all(response);

        sleep(5);
    }
    return 0;
}

bool is_dir(char* name)
{
    DIR * dir;
    if(name == NULL || (dir = opendir(name)) == NULL)
    {
        return FALSE;
    }
    closedir(dir);
    return TRUE;
}

void handler(int sig, siginfo_t *info, void *ucontext)
{
    switch(sig)
    {
        case SIGINT:
            printf("Received SIGINT, stopping...\n");
            stop_all(runners);
            free_all_runners(runners);
            exit(0);
            break;
        case SIGHUP:
            printf("Whats HUP.\n");
            break;
    }
}

void send_sigint_to_instance(char* directory)
{
    pid_t * pids;
    pid_t my_pid = getpid();
    
    pids = pidsof(program_invocation_short_name);

    int index =0;
    while (pids[index] != -1)
    {
        if(pids[index] == my_pid){
            index++;
            continue;
        }
        
        printf("Sending SIGINT to process %s with pid %d.\n", program_invocation_short_name, pids[index]);
        kill(pids[index], SIGINT);
        index++;
    }
    free(pids);
}