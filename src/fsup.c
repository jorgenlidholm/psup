#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#include "directory_watcher.h"
#include "program_watcher.h"

int main(int argc, char ** argv)
{
    if(argc < 1)
        exit(-1);

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
                printf("Option %s with arg %s\n", long_options[option_index].name, optarg);
                directory = malloc(sizeof(char)*strlen(optarg)+1);
                strcpy(directory, optarg);
                break;
            default:
                printf("Usage %s --directory <dir>\n", argv[0]);
        }
    }

    if(directory == NULL)
    {
        printf("Usage %s --directory <dir>\n", argv[0]);
    }

    printf("Fleetech Supvisor watching folder '%s'.\n", directory);

    watched_runner_t ** runners = malloc(sizeof(watched_runner_t*));
    *runners = NULL;
    while (TRUE) {
        char ** response = get_directories_with_runsh(directory);
        update_watched_runners(runners, response);

        int index = 0;
        while(response[index] != NULL)
        {
            printf("Folder %d -> %s\n", index, response[index]);
            index++;
        }

        free_all(response);
        sleep(5);
    }
    return 0;
}
