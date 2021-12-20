#include <sys/inotify.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>

int main() {
    // create a new inotify file descriptor
    // it has an associated watch list
    // read from it to get events
    int inotify_fd = inotify_init();
    if(inotify_fd < 0)
        return 1; // can't create the inotify fd, return 1 to os and exit

    // add a new watch to inotify_fd, monitor the current folder for file/directory creation and deletion
    // returns a watch descriptor. 
    int watch_des = inotify_add_watch(inotify_fd, ".", IN_CREATE|IN_DELETE);
    if(watch_des == -1)
        return 1; // can't create the watch descriptor, return 1 to os and exit

    // create a buffer for at most 10 events
    #define EVENT_STRUCT_SIZE sizeof(struct inotify_event) 
    #define BUFFER_SIZE (10 * (EVENT_STRUCT_SIZE + NAME_MAX + 1))
    char buffer[BUFFER_SIZE];

    // start to monitor
    while(1 == 1) {
        // read from inotify_fd
        int bytesRead = read(inotify_fd, buffer, BUFFER_SIZE), bytesProcessed = 0;
        if(bytesRead < 0) // read error
            return 1;

        while(bytesProcessed < bytesRead) {
            struct inotify_event* event = (struct inotify_event*)(buffer + bytesProcessed);
            if (event->mask & IN_CREATE)
                if (event->mask & IN_ISDIR)
                    printf("directory %s is created.\n", event->name);
                else
                    printf("file %s is created.\n", event->name);
            else if ((event->mask & IN_DELETE))
                if (event->mask & IN_ISDIR)
                    printf("directory %s is deleted.\n", event->name);
                else
                    printf("file %s is deleted.\n", event->name);
            else
                printf("other events happened.\n");

            bytesProcessed += EVENT_STRUCT_SIZE + event->len;
        }
    }

    // will never reach here, for demostration purpose only
    // remove the watch
    inotify_rm_watch(inotify_fd, watch_des);
    close(inotify_fd); // close the fd, will remove all remaining watches 
}