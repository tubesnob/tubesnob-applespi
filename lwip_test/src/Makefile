BINDIR=../bin
TARGET=lwip_test_main
SRC=utils.c init_lwip.c init_sockets.c init_spi.c init_w5500.c lwip_test_main.c 
OBJ=$(SRC:.c=.o)

ifeq ($(DOIIX), true)
LINKDEPS=../../tssocketlib/bin/tssocketlib.o ../../tsspilib/bin/tsspilib.o ../../tslib/bin/tslib.o ../../tsspilib_driver_a2gpio/bin/tsspilib_driver_a2gpio.o  ../../lwip/bin/lwip.o
else
LINKDEPS=/usr/local/lib/libftdi1.dylib ../../tslib/bin/tslib.o ../../tsspilib/bin/tsspilib.o ../../tsspilib_driver_ftdi/bin/tsspilib_driver_ftdi.o ../../tssocketlib/bin/tssocketlib.o ../../lwip/bin/lwip.o
endif 

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
	$(CC) $(CCOPTS) $< cc=-i"../../lwip/src/include/" cc=-i"../../lwip/src/" cc=-i"../../lwip/src/include/"
else
	$(CC) $(CCOPTS) $< $(LINKDEPS) -o $(BINDIR)/$(TARGET) -I ../../lwip/src/include/ -I ../../lwip/src/ -I ../../lwip/src/include/
endif

clean:
	@echo "Cleaning"
	@$(RM) -dR $(BINDIR) $(TARGET) *.dSYM *.o *.o.a *.o.ROOT *.a *.root *.sym *.ROOT *.a.a

$(BINDIR):
	@mkdir $(BINDIR)

dirs: $(BINDIR) 


