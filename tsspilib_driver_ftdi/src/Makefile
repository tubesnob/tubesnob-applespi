LIBFILENAME=tsspilib_ftdi.o

ifeq ($(DOIIX), true)
SRC=tsspilib_ftdi_dummy.c
OBJ=tsspilib_ftdi_dummy.o
else
LINKDEPS=/usr/local/lib/libftdi1.dylib 
SRC=tsspilib_ftdi_main.c
OBJ=tsspilib_ftdi_main.o
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
		$(MAKELIB) $(MAKELIBOPTS) $@ $(OBJ) $(LINKDEPS)
endif

%.o: %.c
	@echo "Compiling C ($@) with ($^) DOIIX=$(DOIIX)"
ifeq ($(DOIIX), true)
	@$(CC) $(CCOPTS) $<
else
	@$(CC) $(CCOPTS) -c $< -o $@
endif

%.ROOT: %.asm
	@echo "Compiling ASM ($@) with ($^) DOIIX=$(DOIIX)"
ifeq ($(DOIIX), true)
	@$(CC) $(CCOPTS) $<
else
	@$(CC) $(CCOPTS) -c $< -o $@
endif

clean:
	@echo "Cleaning"
	$(RM) $(LIBFILENAME) *.o *.sym *.a *.root *.ROOT	

