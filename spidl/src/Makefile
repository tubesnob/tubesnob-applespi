BINDIR=bin
TARGET=spidl
SRC=spidl_main.c
OBJ=$(SRC:.c=.o)
LINKDEPS=../../tslib/src/tslib.o ../../tssocketlib/src/tssocketlib.o ../../tsspilib/src/tsspilib.o

include ../../Makefile.env.include

all: clean $(TARGET)

$(TARGET): dirs $(OBJ)
ifeq ($(DOIIX), true)
		$(eval TRX2=$(OBJ:.o=.a))
		$(LINK) $(LINKOPTS) $(TRX2) $(LINKDEPS) keep=$(BINDIR)/$(TARGET)
else
		#ignore linking phase
endif

%.o: %.c
ifeq ($(DOIIX), true)
	$(CC) $(CCOPTS) $<
else
	$(CC) $(CCOPTS) $< $(LINKDEPS) -o $(BINDIR)/$(TARGET)
endif

clean:
	@echo "Cleaning"
	$(RM) -dR $(BINDIR) $(TARGET) $(OBJ) $(OBJ:.o=.sym) $(OBJ:.o=.a) $(OBJ:.o=.root) $(OBJ:.o=.o.a) $(OBJ:.o=.o.root) *.dSYM

$(BINDIR):
	@mkdir $(BINDIR)

dirs: $(BINDIR) 
