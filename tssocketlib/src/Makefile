LIBFILENAME=tssocketlib.o
SRC=tssocketlib_main.c tssocketlib_w5500.c
OBJ=$(SRC:.c=.o)

ifeq ($(DOIIX), true)
LINKDEPS=../../tslib/src/tslib.o ../../tsspilib/src/tsspilib.o ../../tsspilib_driver_a2gpio/src/tsspilib_a2gpio.o
else
LINKDEPS=/usr/local/lib/libftdi1.dylib ../../tslib/src/tslib.o ../../tsspilib/src/tsspilib.o ../../tsspilib_driver_ftdi/src/tsspilib_ftdi.o
endif 

include ../../Makefile.env.include

all: clean $(LIBFILENAME)

$(LIBFILENAME): $(OBJ)
		@echo "Linking ($@) with ($^) DOIIX=$(DOIIX)"
ifeq ($(DOIIX), true)
		$(eval TRX=$(addprefix +,$(OBJ)))
		$(eval TRX2=$(TRX:.o=.a))
		$(MAKELIB) $(MAKELIBOPTS) $@ $(TRX2)
else
		$(MAKELIB) $(MAKELIBOPTS) $@ $(OBJ)
endif

%.o: %.c
	@echo "Compiling ($@) with ($^) DOIIX=$(DOIIX)"
ifeq ($(DOIIX), true)
	@$(CC) $(CCOPTS) $<
else
	@$(CC) $(CCOPTS) -c $< -o $@
endif

clean:
	@echo "Cleaning"
	$(RM) $(LIBFILENAME) $(LIBFILENAME:.o=.a) $(OBJ) $(OBJ:.o=.sym) $(OBJ:.o=.a) $(OBJ:.o=.root) $(OBJ:.o=.o.a) $(OBJ:.o=.o.root)
	

