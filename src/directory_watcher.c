
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

char** get_directories_with_runsh(char* parent)
{
    char ** response = malloc(sizeof(char**));
    // int total_length = 0;
    int directory_count = 0;

    struct dirent * de;
    DIR * dir = opendir(parent);
    if(dir == NULL)
    {
        printf("Could not open folder '%s'.\n", parent);
        return NULL;
    }
    
    int parent_length = strlen(parent);
    while((de = readdir(dir)) != NULL)
    {
        if(de->d_type != DT_DIR)
            continue;
        if(de->d_name[0] == '.')
            continue;

        /* Build path to run.sh script */
        int length = parent_length + 1 + strlen(de->d_name) + 1;
        char * path = calloc(sizeof(char), length);
        strcpy(path,parent);
        strcat(path,"/");
        strcat(path,de->d_name);
        char * filepath = calloc(sizeof(char), length + 7 + 1);
        strcpy(filepath, path);
        strcat(filepath, "/run.sh");
        
        if(access(filepath, X_OK) == -1)
            continue;
        /* Clean up temporary path */
        free(filepath);

        response = realloc(response, (directory_count + 1) * sizeof(char*));
        response[directory_count] = path;
        directory_count += 1;
    }
    closedir(dir);
    /* NULL terminate */
    void* p = realloc(response, (directory_count+1) * sizeof(char*));
    if(p == NULL)
        return p;
    response = p;
    response[directory_count] = NULL;
    return response;
}


void free_all(char ** directories)
{
    int index = 0;
    while(directories[index] != NULL)
    {
        free(directories[index++]);
    };
    free(directories);
}


