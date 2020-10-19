
#include <cilk/cilk_priority.h>

#include <unistd.h>


CilkPrioCommandPrototype(int, fn, int);

CilkPrioCommandDefine(int, fn, (int input) {
    usleep(1000000);
    return input;
})


int main(int argc, char * argv[]) {
    cilk_enable_diff_prio_spawn_in_this_func();

    cilk::pfuture<int, cilk::Low> * test = new cilk::pfuture<int, cilk::Low>;
    cilk_pfuture_create(test, fn, 10);

    int res = cilk_pfuture_get(test);

    printf("result: %d\n", res);

    return 0;
}
