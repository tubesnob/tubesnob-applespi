TARGETFILENAME=tssocketlib_test_main
SRC=tssocketlib_test_main.c
OBJ=$(SRC:.c=.o)


ifeq ($(DOIIX), true)
LINKDEPS=../../tslib/src/tslib.o ../../tsspilib/src/tsspilib.o ../../tsspilib_driver_a2gpio/src/tsspilib_a2gpio.o ../../tssocketlib/src/tssocketlib.o
else
LINKDEPS=/usr/local/lib/libftdi1.dylib ../../tslib/src/tslib.o ../../tsspilib/src/tsspilib.o ../../tsspilib_driver_ftdi/src/tsspilib_ftdi.o ../../tssocketlib/src/tssocketlib.o
endif 

include ../../Makefile.env.include

all: clean $(TARGETFILENAME)

$(TARGETFILENAME): $(OBJ)
ifeq ($(DOIIX), true)
		$(eval TRX2=$(OBJ:.o=.a))
		$(LINK) $(LINKOPTS) $(TRX2) $(LINKDEPS) keep=$@ 
else
		#ignore linking phase
endif

%.o: %.c
ifeq ($(DOIIX), true)
	$(CC) $(CCOPTS) $<
else
	$(CC) $(CCOPTS) $< $(LINKDEPS) -o $(TARGETFILENAME)
endif

clean:
	@echo "Cleaning"
	$(RM) $(TARGETFILENAME) $(OBJ) $(OBJ:.o=.sym) $(OBJ:.o=.a) $(OBJ:.o=.root) $(OBJ:.o=.o.a) $(OBJ:.o=.o.root)
	

