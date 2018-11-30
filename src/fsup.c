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

#include "directory_watcher.h"
#include "program_watcher.h"

/*
    TODO: Should call stop on run.sh when trying to restart a process.
            To ensure things are cleaned up according to script.
*/

watched_runner_t ** runners;

bool is_dir(char* name);
void handler(int sig, siginfo_t *info, void *ucontext);

void print_usage(char * name)
{
    printf("Usage %s --directory <dir>\n", name);
}

int main(int argc, char ** argv)
{
    struct sigaction act;
    printf ("### Fleetech Supervisor ###\n\n");
    if(argc < 1)
    {
        printf("Wrong argument count.\n");
        print_usage(argv[0]);
        exit(-1);
    }

    static struct option long_options[] = {
        {"directory", required_argument, 0, 0},
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
            default:
                print_usage(argv[0]);
        }
    }

    if(!is_dir(directory))
    {
        printf("%s is not a valid directory, or you don't have access.\n", directory);
        print_usage(argv[0]);
        exit (-1);
    }

    /* Sätt upp skydd mot Ctrl-C */

    
    memset (&act, '\0', sizeof(act));
    act.sa_handler = (void*)&handler;
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGHUP, &act, NULL);


    printf("Watching folder '%s'.\n", directory);

    runners = malloc(sizeof(watched_runner_t*));
    *runners = NULL;
    while (TRUE) {
        char ** response = get_directories_with_runsh(directory);
        update_watched_runners(runners, response);
        free_all(response);
        sleep(5);
    }
    return 0;
}

bool is_dir(char* name)
{
    DIR * dir;
    if((dir = opendir(name)) == NULL)
    {
        return FALSE;
    }
    closedir(dir);
    return TRUE;
}

void handler(int sig, siginfo_t *info, void *ucontext)
{
    if (sig == SIGINT)
    {
        stop_all(runners);
        free_all_runners(runners);
    }

    exit(0);
    // stop_all(runners);
}