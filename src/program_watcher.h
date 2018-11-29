#ifndef PROGRAM_RUNNER_H
#define PROGRAM_RUNNER_H
    #include <unistd.h>
    typedef char bool;
    const bool FALSE;
    const bool TRUE;

    typedef struct watched_runner {
        pid_t pid;
        char * program_name;
        char * path;
        int restart_count;
    } watched_runner_t;

    pid_t spawn(watched_runner_t * runner);
    bool is_running(watched_runner_t * runner);
    void update_watched_runners (watched_runner_t** runners, char ** directories);
    
#endif /* PROGRAM_RUNNER_H */