#ifndef __APPLE__
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <errno.h>

char*  watchfd_to_name[512]={0};
size_t pwnage_count[512]={0};
time_t pwnage_times[512]={0};

char* print_mask(int mask) {
#define CASE_RETURN(x) case x: return #x;
    switch(mask) {
        CASE_RETURN(IN_ACCESS)
        CASE_RETURN(IN_ATTRIB)
        CASE_RETURN(IN_CLOSE_WRITE)
        CASE_RETURN(IN_CLOSE_NOWRITE)
        CASE_RETURN(IN_CREATE)
        CASE_RETURN(IN_DELETE)
        CASE_RETURN(IN_DELETE_SELF)
        CASE_RETURN(IN_MODIFY)
        CASE_RETURN(IN_MOVE_SELF)
        CASE_RETURN(IN_MOVED_FROM)
        CASE_RETURN(IN_MOVED_TO)
        CASE_RETURN(IN_OPEN)
    }
    return "UNKNOWN";
}

// default compilation options

#ifndef TIMEOUT
#define TIMEOUT 60
#endif

#ifndef INOTIFY_FLAG
#define INOTIFY_FLAG IN_ACCESS
#endif

#ifndef INOTIFY_TARGETS
#define INOTIFY_TARGETS {"$HOME/flag",0}
#endif

#ifndef INOTIFY_STDIN
#define INOTIFY_STDIN 0
#endif

int
main
(
    int     argc,
    char    **argv
)
{
    union {
    struct inotify_event event;
    char   buffer[1024];
    }  u;
    int     pgid     = getpgid(0);
    int     kill_pid = -pgid;
    int     inotify_fd  = inotify_init();

    memset(&u.buffer, 0, sizeof(u.buffer));

    if(inotify_fd < 0) {
        perror("init");
        exit(1);
    }

    dprintf(2, "Started\n");


    //
    // Get arguments from compilation variables
    //
    int timeout = TIMEOUT;
    int flag = INOTIFY_FLAG;
    char *targets[512] = INOTIFY_TARGETS;

    if(!targets[0]) {
        targets[0] = (char*)malloc(4096);
        targets[1] = NULL;

        if(!targets[0]) {
            perror("malloc");
            exit(1);
        }

        #if INOTIFY_STDIN
        {
            //
            // If no compiled targets, read one
            //

            int len = scanf("%4095s",targets[0]);

            if(len < 0) {
                perror("read");
                exit(1);
            }
        }
        #else
        {
            //
            // If no compiled targets and no stdin, read /home/$USER/flag
            //

            #include <pwd.h>
            #include <unistd.h>
            #include <sys/types.h>
            struct passwd *pwnam = getpwuid(getuid());

            if(pwnam == NULL) {
                perror("pwuid");
                exit(1);
            }

            int len = sprintf(targets[0],"/home/flags/%s",pwnam->pw_name);

            if(len < 0) {
                perror("sscanf");
                exit(1);
            }
        }
        #endif
    }

    for(int arg = 0; targets[arg]; arg++) {
        int     watch_fd = inotify_add_watch(inotify_fd, targets[arg], flag);

        if(watch_fd < 0) {
            perror("watch");
        }

        watchfd_to_name[watch_fd] = realpath(targets[arg], NULL);
    }

    while(0 < read(inotify_fd, &u.buffer, sizeof(u.buffer))) {
        int wd = u.event.wd;
        char* filename = watchfd_to_name[wd];
        struct timeval now;
        time_t nt;
        struct tm *nowtm;
        char tmbuf[64], dtbuf[64];

        pwnage_count[wd]++;

        dprintf(2, "%s (%lu)\n", filename, pwnage_count[wd]);

        if(pwnage_times[wd] + timeout > time(0)) {
            continue;
        }

        if(pwnage_count[wd] > 1) {
            dprintf(1,
                    "inotify pwned %s \"%s\" %zu times in %ld seconds\n",
                    print_mask(u.event.mask),
                    filename,
                    pwnage_count[wd],
                    pwnage_times[wd] ? time(0) - pwnage_times[wd] : -1);
        } else {
            gettimeofday(&now, NULL);
            nt = now.tv_sec;
            nowtm = localtime(&nt);
            strftime(tmbuf, sizeof(tmbuf), "%Y-%m-%d %H:%M:%S", nowtm);
            snprintf(dtbuf, sizeof(dtbuf), "%s.%06ld", tmbuf, now.tv_usec);
            dprintf(1,
                    "inotify pwned %s %s \"%s\"\n",
                    dtbuf,
                    print_mask(u.event.mask),
                    filename);
        }

        pwnage_times[wd] = time(0);
        pwnage_count[wd] = 0;
    }

    dprintf(2, "Exiting: %s\n", strerror(errno));
    return 0;
}
#else
#include <stdio.h>
int main()
{
    puts("inotify does not work on OSX");
}
#endif
