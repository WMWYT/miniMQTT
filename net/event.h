#ifndef EVENT_H
#define EVENT_H
#include "net.h"

extern int server_sock, client_sock;
extern int epfd;

extern struct epoll_event * epoll_events;
extern struct epoll_event event;

int event_handle(int * packet_len, char * buff, int fd);

#endif