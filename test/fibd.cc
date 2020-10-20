
#include <cilk/cilk_priority.h>

#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <srv.h>


static struct server * g_srv;


static void daemonize() {
    pid_t child = fork();

    if (child < 0) {
        fprintf(stderr, "failed to daemonize, reason: %s\n", strerror(errno));
        exit(child);
    }

    if (child != 0) {
        // from aerospike-server, to prevent cleanups by parent
        _exit(0);
    }
}


static void sig_call_init_server_shutdown(int sig) {
    init_server_shutdown(g_srv);
}


int main(int argc, char * argv[]) {
    struct server srv;

    int ret;

    cilk_enable_diff_prio_spawn_in_this_func();

    // daemonize();

    if ((ret = initialize_server(&srv, 8080)) != 0) {
        return ret;
    }

    g_srv = &srv;
    __atomic_thread_fence(__ATOMIC_SEQ_CST);

    signal(SIGUSR2, sig_call_init_server_shutdown);

    cilk_pspawn_void(AcceptP, accept_loop, &srv);

    //usleep(1000000);
    //kill(getpid(), SIGUSR2);

    cilk_psync;

    // init_server_shutdown(&srv);

    return 0;
}
