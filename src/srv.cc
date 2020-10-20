
#include <cilk/cilk_priority.h>

#include <errno.h>
#include <signal.h>
#include <srv.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <client.h>


static int connect_server(struct server * srv, int listenfd) {
    int ret;

    if ((ret = bind(listenfd, (struct sockaddr *) &srv->inet_addr,
             sizeof(srv->inet_addr)))
        < 0) {
        fprintf(stderr, "Unable to bind to socket on fd %d, reason: %s\n",
            listenfd, strerror(errno));
        return ret;
    }

    if ((ret = listen(listenfd, srv->backlog)) < 0) {
        fprintf(stderr, "Unable to listen to socket on fd %d, reason: %s\n",
            listenfd, strerror(errno));
        return ret;
    }

    srv->listenfd = listenfd;

    return 0;
}


int initialize_server(struct server * srv, int port) {
    int sockfd;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        fprintf(stderr, "Unable to open socket, reason: %s\n", strerror(errno));
        return -1;
    }

    srv->inet_addr.sin_family      = AF_INET;
    srv->inet_addr.sin_addr.s_addr = INADDR_ANY;
    srv->inet_addr.sin_port        = htons(port);

    if (connect_server(srv, sockfd) != 0) {
        close(sockfd);
        return -1;
    }

    return 0;
}



int init_server_shutdown(struct server * srv) {
    srv->running = 0;
    close(srv->listenfd);
    return 0;
}



CilkPrioCommandDefine(void, accept_loop, (struct server * srv), {
    struct sockaddr_storage addr;
    socklen_t               addrlen = sizeof(addr);

    int sfd;
    int stop = 0;

    cilk_enable_spawn_in_this_func();

    while (srv->running) {
        sfd = cilk_accept4_sync(
            srv->listenfd, (struct sockaddr *) &addr, &addrlen, SOCK_NONBLOCK);

        printf("accept returns %d, error %s\n", sfd, strerror(errno));
        if (sfd >= 0) {
            cilk_pspawn_void(ClientP, handle_client_conn, srv, sfd);
        }
    }
    cilk_psync;
    return;
});

CilkPrioCommandInstantiateAtPrio(accept_loop, AcceptP);
