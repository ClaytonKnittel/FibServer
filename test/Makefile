include ../common.mk


ODIR=.obj
PODIR=.pobj

SRC=$(shell find . -type f -name '*.c')
OBJ=$(patsubst %.c,$(ODIR)/%.o,$(SRC))
EXE=$(patsubst %.c,$(BIN_DIR)/%,$(SRC))

PSRC=$(shell find . -type f -name '*.cc')
POBJ=$(patsubst %.cc,$(PODIR)/%.o,$(PSRC))
PEXE=$(patsubst %.cc,$(BIN_DIR)/%,$(PSRC))

$(shell mkdir -p $(ODIR))
$(shell mkdir -p $(PODIR))

DEPFILES=$(SRC:%.c=$(ODIR)/%.d)
PDEPFILES=$(PSRC:%.cc=$(PODIR)/%.d)

IFLAGS=-I$(BASE_DIR)/include


# to prevent files from being auto-deleted by make after build completes
.SECONDARY:

.PHONY: all
all: $(EXE) $(PEXE)

$(BIN_DIR)/%: $(ODIR)/%.o $(SRCLIB)
	$(CC) $< -o $@ $(SRCLIB)

$(BIN_DIR)/%: $(PODIR)/%.o $(SRCLIB)
	$(CPPC) $< -o $@ $(LDFLAGS) $(SRCLIB)

$(ODIR)/%.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@ $(IFLAGS)

$(PODIR)/%.o: %.cc
	$(CPPC) $(CPPFLAGS) $< -c -o $@ $(IFLAGS)


-include $(wildcard $(DEPFILES))
-include $(wildcard $(PDEPFILES))

.PHONY: clean
clean:
	rm -rf $(ODIR) $(PODIR)

