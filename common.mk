
CILK_DIR=/project/orwell/home/cknittel/interactive-cilk

CC=$(CILK_DIR)/Tapir/src/build/bin/clang++


COMMON=-std=c++17 -g

CFLAGS=$(COMMON) -fcilkplus -fno-omit-frame-pointer -Iinclude/ -I$(CILK_DIR)/interactive-cilk/runtime/config/x86 -I$(CILK_DIR)/interactive-cilk/include

#ICILK_A=$(CILK_DIR)/interactive-cilk/.libs/libcilkrts.a
#ICILK_A=$(CILK_DIR)/Tapir/src/build/lib/libcilkrts.so
ICILK_A=$(CILK_DIR)/interactive-cilk/.libs/libcilkrts.a
#ICILK_A=-lcilkrts

LDFLAGS=$(ICILK_A) -ltcmalloc -lrt -ldl -lpthread -lnuma -lpthread -lm -lrt

