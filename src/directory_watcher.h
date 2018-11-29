#ifndef DIRECTORY_WATCHER_H
#define DIRECTORY_WATCHER_H

char** get_directories_with_runsh(char* parent);

void free_all(char ** directories);
#endif /* DIRECTORY_WATCHER_H */