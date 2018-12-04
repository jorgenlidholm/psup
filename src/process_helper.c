#include <stddef.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

#include "program_watcher.h"

bool is_numeric(char * name);
bool is_command_dir(char* dir, char * command);

pid_t * pidsof(char * command)
{
    pid_t * pids = NULL;
    int count = 0;
    DIR * dir = opendir("/proc");

    if(dir == NULL)
    {
        printf("Could not open folder '/proc'.\n");
        return NULL;
    }
    
    struct dirent * de;
    while((de = readdir(dir)) != NULL)
    {
        if(de->d_type != DT_DIR)
            continue;
        if(de->d_name[0] == '.')
            continue;

        
        if(!is_numeric((char*)de->d_name))
            continue;

        if(!is_command_dir((char*)de->d_name, command))
            continue;

        if (pids == NULL) {
            pids = malloc(sizeof(pid_t));
            count = 1;
        } else {
            pids = realloc(pids,sizeof(pid_t)*++count);
        }
        sscanf(de->d_name, "%d", &pids[count-1]);
    }

    closedir(dir);

    pids = realloc(pids,sizeof(pid_t)*(count+1));
    pids[count] = -1;
    return pids;
}

bool is_numeric(char * name)
{
    for (int i = 0; i < strlen(name); i++)
    {
        if(!isdigit(name[i]))
            return FALSE;
    }
    return TRUE;
}

bool is_command_dir(char* dir, char * command)
{
    char filename[2048];
    char cname[1024];
    memset(filename, 0, 2048);
    strcpy(filename, "/proc/");
    strcat(filename, dir);
    strcat(filename, "/comm");
    int fd = open(filename, O_RDONLY);

    ssize_t count = read(fd, cname, 1024);

    if(count > 0) {
        cname[count-1] = '\0';
        return !strcmp(command, cname);
    }
    return FALSE;
}

