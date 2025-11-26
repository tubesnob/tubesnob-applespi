BINDIR=../bin
TARGET=tsiplib_test
SRC=tsiplib_test_main.c
OBJ=$(SRC:.c=.o)

ifeq ($(DOIIX), true)
LINKDEPS=../../tsspilib/bin/tsspilib.o ../../tsiplib/bin/tsiplib.o  ../../tslib/bin/tslib.o ../../tsspilib_driver_a2gpio/bin/tsspilib_driver_a2gpio.o 
else
LINKDEPS=/opt/homebrew/Cellar/libftdi/1.5_2/lib/libftdi1.dylib ../../tslib/bin/tslib.o ../../tsiplib/bin/tsiplib.o ../../tsiplib_driver_w5500/bin/tsiplib_driver_w5500.o  ../../tsspilib/bin/tsspilib.o ../../tsspilib_driver_ftdi/bin/tsspilib_driver_ftdi.o
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


