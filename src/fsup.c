#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char ** args)
{
    printf("Hello world.\n");

    pid_t pid = fork();

    if(pid == 0)
    {
        char * exe = "/bin/ls";
        char * arg[] = { "/bin/ls", "-la", NULL};
        int exec_result = execvp(exe, arg);
        if(exec_result != 0)
        {
            perror("execvp");
            return -1;
        }
        return 0;
    }

    int wstatus;
    int result = waitpid(pid, &wstatus, 0);

    if(result == pid)
    {
        printf("Successfully called 'ls'\n");
    }
    else{
        printf ("This succs\n");
    }
}