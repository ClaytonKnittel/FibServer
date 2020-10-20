
#include <cilk/cilk_priority.h>

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


#define fprintf(file, fmt, ...) \
    fprintf(file, "\033[31m" fmt "\033[0m", __VA_ARGS__)


int main(int argc, char * argv[]) {
    struct sockaddr_in addr;

    int sfd;
    int port = 8080;

    addr.sin_family = AF_INET;
    inet_aton("127.0.0.1", &addr.sin_addr);
    addr.sin_port = htons(port);

    sfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sfd < 0) {
        fprintf(stderr, "Unable to initialize socket, reason: %s\n",
            strerror(errno));
        return sfd;
    }

    if (connect(sfd, (struct sockaddr *) &addr, sizeof(addr)) != 0) {
        fprintf(
            stderr, "Unable to connect to host, reason: %s\n", strerror(errno));
        return -1;
    }

    usleep(1000000);
    close(sfd);

    return 0;
}
