include common.mk

SDIR=src
ODIR=.obj
BDIR=bin

EXE=scr

SRC=$(shell find $(SDIR) -name "*.cc" -type f)
OBJ=$(patsubst $(SDIR)/%.cc,$(ODIR)/%.o,$(SRC))
DEP=$(wildcard $(SDIR)/*.h)

DIRS=$(shell find $(SDIR) -type d)
OBJDIRS=$(patsubst $(SDIR)/%,$(ODIR)/%,$(DIRS))

$(shell mkdir -p $(ODIR))
$(shell mkdir -p $(OBJDIRS))
$(shell mkdir -p $(BDIR))

DEPFILES=$(SRC:$(SDIR)/%.cc=$(ODIR)/%.d)

$(BDIR)/$(EXE): $(OBJ)
	$(CC) -flto $(OBJ) -o $(BDIR)/$(EXE) $(LDFLAGS)

$(ODIR)/%.o: $(SDIR)/%.cc
	$(CC) $(CFLAGS) -c -o $@ $<

-include $(wildcard $(DEPFILES))

.PHONY: clean
clean:
	rm -rf $(ODIR) $(BDIR)


