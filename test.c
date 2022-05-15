#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/socket.h>

enum { PORT = 80, BACKLOG = 16384, EV_LEN = 2048, BUF_LEN = 65535 };

static const char* index_html = "HTTP/1.0 200 OK\r\n" \
                                 "Content-Length: 86\r\n\r\n" \
                                 "<!DOCTYPE html>" \
                                 "<html><head>Hello, world!</head><body><h1>cdn-ish...</h1></body></html>";

static char buf[BUF_LEN] = { 0, };

static struct kevent ev_list[EV_LEN];

int main(void) {
    int fd, kqfd, serverfd, clientfd, ev_count, i, flags;
    struct kevent ev_set;
    struct sockaddr_in serveraddr, clientaddr;
    int clientaddr_size;

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr("0.0.0.0");
    serveraddr.sin_port = htons((short)PORT);
    if ((serverfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }
    if (bind(serverfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) < 0) {
        perror("bind");
        return EXIT_FAILURE;
    }
    if (listen(serverfd, BACKLOG) < 0) {
        perror("listen");
        return EXIT_FAILURE;
    }

    kqfd = kqueue();
    EV_SET(&ev_set, serverfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kqfd, &ev_set, 1, NULL, 0, NULL) < 0) {
        perror("kevent");
        return EXIT_FAILURE;
    }

    for (;;) {
        ev_count = kevent(kqfd, NULL, 0, ev_list, EV_LEN, NULL);

        for (i = 0; i < ev_count; i++) {
            fd = (int)ev_list[i].ident;

            if (fd < 0) continue;
            if (fd == serverfd) {
                clientfd = accept(fd, (struct sockaddr*)&clientaddr, &clientaddr_size);
                if (clientfd < 0) {
                    if (errno == EMFILE) {
                        continue;
                    }
                    perror("accept");
                    close(fd);
                    return EXIT_FAILURE;
                }
                EV_SET(&ev_set, clientfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
                kevent(kqfd, &ev_set, 1, NULL, 0, NULL);
                if ((flags = fcntl(clientfd, F_GETFL, 0)) < 0) {
                    perror("fcntl");
                    close(clientfd);
                    close(fd);
                }
                if (fcntl(clientfd, F_SETFL, flags | O_NONBLOCK) < 0) {
                    perror("fcntl");
                    close(clientfd);
                    close(fd);
                }
                EV_SET(&ev_set, clientfd, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, NULL);
                kevent(kqfd, &ev_set, 1, NULL, 0, NULL);
            } else if (ev_list[i].filter == EVFILT_READ) {
                int len;

                memset(buf, 0, sizeof(buf));
                if ((len = recv(fd, buf, sizeof(buf), 0)) != 0) {
                    if (errno == EAGAIN) {
                        printf("EAGAIN\n");
                    }
                }
            } else if (ev_list[i].filter == EVFILT_WRITE) {
                int offset, len;

                offset = (int)ev_list[i].udata;
                len = 0;
                if ((len = send(fd, index_html + offset, strlen(index_html) - offset, 0)) != 0) {
                    if (errno == EAGAIN) {
                        EV_SET(&ev_set, fd, EVFILT_WRITE, EV_ADD | EV_ONESHOT, 0, 0, (void*)(offset + len));
                        kevent(kqfd, &ev_set, 1, NULL, 0, NULL);
                    } else {
                        EV_SET(&ev_set, fd, 0, EV_DELETE, 0, 0, NULL);
                        ev_list[i].ident = -1;
                        close(fd);
                    }
                }
            }
        }
    }
    return EXIT_SUCCESS;
}