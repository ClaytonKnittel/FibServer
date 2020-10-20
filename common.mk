
BASE_DIR=${CURDIR}

CILK_DIR=/project/orwell/home/cknittel/interactive-cilk

AR=ar
CC=gcc
CPPC=$(CILK_DIR)/Tapir/src/build/bin/clang++


DEBUG=0


COMMON=-std=c++17 -g

_CFLAGS=-g -Iinclude/
_CPPFLAGS=$(COMMON) -fcilkplus -fno-omit-frame-pointer -Iinclude/ -I$(CILK_DIR)/interactive-cilk/runtime/config/x86 -I$(CILK_DIR)/interactive-cilk/include

ifeq ($(DEBUG),1)
CFLAGS=$(_CFLAGS) -O0
CPPFLAGS=$(_CPPFLAGS) -O0
else
CFLAGS=$(_CFLAGS) -O3
CPPFLAGS=$(_CPPFLAGS) -O3
endif


#ICILK_A=$(CILK_DIR)/interactive-cilk/.libs/libcilkrts.a
#ICILK_A=$(CILK_DIR)/Tapir/src/build/lib/libcilkrts.so
ICILK_A=$(CILK_DIR)/interactive-cilk/.libs/libcilkrts.a
#ICILK_A=-lcilkrts

LDFLAGS=$(ICILK_A) -ltcmalloc -lrt -ldl -lpthread -lnuma -lpthread -lm -lrt

