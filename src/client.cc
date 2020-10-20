
#include <unistd.h>

#include <client.h>


CilkPrioCommandDefine(void, handle_client_conn, (struct server * srv, int clientfd), {

    printf("doing stuff w/ client %d\n", clientfd);

    close(clientfd);
    return;
});


CilkPrioCommandInstantiateAtPrio(handle_client_conn, ClientP);

