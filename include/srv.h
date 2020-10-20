#pragma once

#include <netinet/in.h>


struct server {
    struct sockaddr_in inet_addr;

    int listenfd;

    int backlog;

    volatile int running;
};


int initialize_server(struct server * srv, int port);

int init_server_shutdown(struct server * srv);


CilkPrioCommandPrototype(void, accept_loop, struct server *);
CilkPrioCommandForwardDeclareAtPrio(accept_loop, cilk::Low);


