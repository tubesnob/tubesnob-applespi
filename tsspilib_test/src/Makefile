TARGETFILENAME=tsspilib_test
SRC=tsspilib_test_main.c
OBJ=$(SRC:.c=.o)
OUTDIR=bin

ifeq ($(DOIIX), true)
LINKDEPS=../../tslib/src/tslib.o ../../tsspilib/src/tsspilib.o ../../tsspilib_driver_a2gpio/src/tsspilib_a2gpio.o
else
LINKDEPS=/usr/local/lib/libftdi1.dylib ../../tslib/src/tslib.o ../../tsspilib/src/tsspilib.o ../../tsspilib_driver_ftdi/src/tsspilib_ftdi.o
endif 

include ../../Makefile.env.include

all: clean $(TARGETFILENAME)

$(TARGETFILENAME): dirs $(OBJ)
ifeq ($(DOIIX), true)
		$(eval TRX2=$(OBJ:.o=.a))
		$(LINK) $(LINKOPTS) $(TRX2) $(LINKDEPS) keep=$(OUTDIR)/$(TARGETFILENAME)
else
		#ignore linking phase
endif

%.o: %.c
	echo "Compiling $@ with CCOPTS=$(CCOPTS)"
ifeq ($(DOIIX), true)
	$(CC) $(CCOPTS) $<
else
	$(CC) $(CCOPTS) $< $(LINKDEPS) -o $(OUTDIR)/$(TARGETFILENAME)
endif

clean:
	@echo "Cleaning"
	$(RM) -dR $(OUTDIR) $(TARGETFILENAME) $(OBJ) $(OBJ:.o=.sym) $(OBJ:.o=.a) $(OBJ:.o=.root) $(OBJ:.o=.o.a) $(OBJ:.o=.o.root) *.dSYM

$(OUTDIR):
	mkdir $(OUTDIR)

dirs: $(OUTDIR) 
