
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <client.h>

#define MAXLINE 4


static void respond_to_client(int clientfd, int input) {
    input *= 2;
    io_future fut = cilk_write(clientfd, &input, sizeof(input));
    io_future_result res = cilk_iosync(&fut);

    if (res.ret_val == -1) {
        fprintf(stderr, "Error writing to socket %d, reason: %s\n",
                clientfd, strerror(errno));
    }
}


CilkPrioCommandDefine(void, handle_client_conn, (struct server * srv, int clientfd), {
    char buf[MAXLINE];

    printf("doing stuff w/ client %d\n", clientfd);

    int offset = 0;
    for (;;) {
        // read number from client
        io_future fut = cilk_read(clientfd, buf + offset, sizeof(buf) - offset);
        io_future_result res = cilk_iosync(&fut);
        if (res.ret_val > 0) {
            offset += res.ret_val;
            if (offset < 4) {
                continue;
            }
            else if (offset > 4) {
                break;
            }
            else {
                respond_to_client(clientfd, *((int *) buf));
                break;
            }
        }
        else if (res.ret_val < 0) {
            fprintf(stderr, "error reading from socket %d, reason: %s\n",
                    clientfd, strerror(errno));
            break;
        }
        else {
            break;
        }
    }

    close(clientfd);
    return;
});


CilkPrioCommandInstantiateAtPrio(handle_client_conn, ClientP);

