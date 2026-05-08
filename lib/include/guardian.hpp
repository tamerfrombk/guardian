#pragma once

#include <iostream>

#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <unistd.h>

namespace gdn {

// Taken VERBATIM from `man inotify`

/* Read all available inotify events from the file descriptor 'fd'.
         wd is the table of watch descriptors for the directories in argv.
         argc is the size of wd and argv.
         argv is the list of watched directories.
         Entry 0 of wd and argv is unused.  */

[[nodiscard]] static auto handle_events(int fd, int *wd, int argc, char *argv[]) -> int {
    /* Some systems cannot read integer variables if they are not
       properly aligned.  On other systems, incorrect alignment may
       decrease performance.  Hence, the buffer used for reading from
       the inotify file descriptor should have the same alignment as
       struct inotify_event.  */

    char buf[4096] __attribute__((aligned(__alignof__(struct inotify_event))));
    const struct inotify_event *event;
    ssize_t size;

    /* Loop while events can be read from inotify file descriptor.  */

    for (;;) {

        /* Read some events.  */

        size = read(fd, buf, sizeof(buf));
        if (size == -1 && errno != EAGAIN) {
            perror("read");
            return EXIT_FAILURE;
        }

        /* If the nonblocking read() found no events to read, then
           it returns -1 with errno set to EAGAIN.  In that case,
           we exit the loop.  */

        if (size <= 0)
            break;

        /* Loop over all events in the buffer.  */

        for (char *ptr = buf; ptr < buf + size; ptr += sizeof(struct inotify_event) + event->len) {

            event = (const struct inotify_event *)ptr;

            /* Print event type.  */

            if (event->mask & IN_OPEN)
                printf("IN_OPEN: ");
            if (event->mask & IN_CLOSE_NOWRITE)
                printf("IN_CLOSE_NOWRITE: ");
            if (event->mask & IN_CLOSE_WRITE)
                printf("IN_CLOSE_WRITE: ");
            if (event->mask & IN_CREATE)
                printf("IN_CREATE: ");
            if (event->mask & IN_DELETE)
                printf("IN_DELETE: ");
            if (event->mask & IN_DELETE_SELF)
                printf("IN_DELETE_SELF: ");
            if (event->mask & IN_MOVED_FROM)
                printf("IN_MOVED_FROM: ");
            if (event->mask & IN_MOVED_TO)
                printf("IN_MOVED_TO: ");
            if (event->mask & IN_MOVE_SELF)
                printf("IN_MOVE_SELF: ");

            /* Print the name of the watched directory.  */
            for (auto i = 1; i < argc; ++i) {
                if (wd[i] == event->wd) {
                    printf("%s/", argv[i]);
                    break;
                }
            }

            /* Print the name of the file.  */

            if (event->len)
                printf("%s", event->name);

            /* Print type of filesystem object.  */

            if (event->mask & IN_ISDIR)
                printf(" [directory]\n");
            else
                printf(" [file]\n");
        }
    }

    return 0;
}

constexpr auto run_impl(int argc, char **argv) -> int {
    char buf;
    int fd, i, poll_num;
    int *wd;
    nfds_t nfds;
    struct pollfd fds[2];

    if (argc < 2) {
        printf("Usage: %s PATH [PATH ...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    printf("Press ENTER key to terminate.\n");

    /* Create the file descriptor for accessing the inotify API.  */

    fd = inotify_init1(IN_NONBLOCK);
    if (fd == -1) {
        perror("inotify_init1");
        return EXIT_FAILURE;
    }

    /* Allocate memory for watch descriptors.  */

    wd = static_cast<int *>(calloc(argc, sizeof(int)));
    if (wd == NULL) {
        perror("calloc");
        return EXIT_FAILURE;
    }

    /* Mark directories for events
       - file was opened
       - file was closed */

    for (i = 1; i < argc; i++) {
        wd[i] = inotify_add_watch(fd, argv[i],
                                  IN_OPEN | IN_CLOSE | IN_CREATE | IN_DELETE | IN_DELETE_SELF | IN_MOVE | IN_MOVE_SELF);
        if (wd[i] == -1) {
            fprintf(stderr, "Cannot watch '%s': %s\n", argv[i], strerror(errno));

            return EXIT_FAILURE;
        }
        // IF is dir: recursively add
    }

    /* Prepare for polling.  */
    nfds = 2;

    fds[0].fd = STDIN_FILENO; /* Console input */
    fds[0].events = POLLIN;

    fds[1].fd = fd; /* Inotify input */
    fds[1].events = POLLIN;

    /* Wait for events and/or terminal input.  */

    printf("Listening for events.\n");
    while (1) {
        poll_num = poll(fds, nfds, -1);
        if (poll_num == -1) {
            if (errno == EINTR)
                continue;
            perror("poll");

            return EXIT_FAILURE;
        }

        if (poll_num > 0) {
            if (fds[0].revents & POLLIN) {
                /* Console input is available.  Empty stdin and quit.  */
                while (read(STDIN_FILENO, &buf, 1) > 0 && buf != '\n')
                    continue;
                break;
            }

            if (fds[1].revents & POLLIN) {
                /* Inotify events are available.  */
                if (const auto ret = handle_events(fd, wd, argc, argv); ret) {
                    return ret;
                }
            }
        }
    }

    printf("Listening for events stopped.\n");

    /* Close inotify file descriptor.  */

    close(fd);

    free(wd);

    return EXIT_SUCCESS;
}

[[nodiscard]] constexpr auto run(int argc, char **argv) -> int {
    std::cout << argv[0] << " called with: " << "\n";
    for (auto i = 1; i < argc; ++i) {
        std::cout << "\t" << argv[i] << "\n";
    }

    return run_impl(argc, argv);
}

} // namespace gdn
