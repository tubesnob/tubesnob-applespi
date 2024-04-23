BINDIR=../bin
TARGET=tslib_test_main
SRC=tslib_test_main.c
OBJ=$(SRC:.c=.o)
LINKDEPS=../../tslib/bin/tslib.o

include ../../Makefile.env.include

all: clean $(TARGET)

$(TARGET): dirs $(OBJ)
ifeq ($(DOIIX), true)
		$(eval TRX2=$(OBJ:.o=.a))
		$(LINK) $(LINKOPTS) $(TRX2) $(LINKDEPS) keep=$(BINDIR)/$(TARGET)
else
		@#ignore linking phase
endif

%.o: %.c
ifeq ($(DOIIX), true)
	$(CC) $(CCOPTS) $<
else
	$(CC) $(CCOPTS) $< $(LINKDEPS) -o $(BINDIR)/$(TARGET)
endif

clean:
	@echo "Cleaning"
	@$(RM) -dR $(BINDIR) $(TARGET) *.dSYM *.o *.o.a *.o.ROOT *.a *.root *.sym *.ROOT

$(BINDIR):
	@mkdir $(BINDIR)

dirs: $(BINDIR) 
