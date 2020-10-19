include common.mk

SDIR=src
ODIR=.obj
BDIR=bin
LDIR=lib

EXE=fibd
SRCLIB=srclib.a

CSRC=$(shell find $(SDIR) -name "*.c" -type f)
COBJ=$(patsubst $(SDIR)/%.c,$(ODIR)/%.o,$(CSRC))

SRC=$(shell find $(SDIR) -name "*.cc" -type f)
OBJ=$(patsubst $(SDIR)/%.cc,$(ODIR)/%.o,$(SRC))
DEP=$(wildcard $(SDIR)/*.h)

DIRS=$(shell find $(SDIR) -type d)
OBJDIRS=$(patsubst $(SDIR)/%,$(ODIR)/%,$(DIRS))

$(shell mkdir -p $(ODIR))
$(shell mkdir -p $(OBJDIRS))
$(shell mkdir -p $(BDIR))
$(shell mkdir -p $(LDIR))

CDEPFILES=$(CSRC:$(SDIR)/%.c=$(ODIR)/%.d)
DEPFILES=$(SRC:$(SDIR)/%.cc=$(ODIR)/%.d)

all: $(BDIR)/$(EXE) $(LDIR)/$(SRCLIB)

.PHONY: test
test: $(LDIR)/$(SRCLIB)
	(make -C test/ BASE_DIR=$(BASE_DIR) BIN_DIR=$(BASE_DIR)/$(BDIR) SRCLIB=$(BASE_DIR)/$(LDIR)/$(SRCLIB))

$(BDIR)/$(EXE): $(OBJ) $(COBJ)
	$(CPPC) -flto $(OBJ) $(COBJ) -o $(BDIR)/$(EXE) $(LDFLAGS)

$(LDIR)/$(SRCLIB): $(OBJ) $(COBJ)
	$(AR) -rcs $@ $(OBJ) $(COBJ)

$(ODIR)/%.o: $(SDIR)/%.cc
	$(CPPC) $(CPPFLAGS) -c -o $@ $<

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

-include $(wildcard $(DEPFILES))
-include $(wildcard $(CDEPFILES))

.PHONY: clean
clean:
	rm -rf $(ODIR) $(BDIR) $(LDIR)
	(make -C test/ clean)


