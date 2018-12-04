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
        bool dont_run;
        time_t file_time;
    } watched_runner_t;

    watched_runner_t** update_watched_runners (watched_runner_t** runners, char ** directories);
    void free_all_runners(watched_runner_t** runners);
    void stop_all(watched_runner_t** runners);
#endif /* PROGRAM_RUNNER_H */