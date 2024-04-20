LIBFILENAME=tsspilib_a2gpio.o

ifeq ($(DOIIX), true)
SRC=tsspilib_a2gpio_main.c tsspilib_a2gpio_spireadb.asm tsspilib_a2gpio_spisendb.asm
OBJ=tsspilib_a2gpio_main.o tsspilib_a2gpio_spireadb.ROOT tsspilib_a2gpio_spisendb.ROOT
else
SRC=tsspilib_a2gpio_dummy.c
OBJ=tsspilib_a2gpio_dummy.o
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
	@echo "Compiling C ($@) with ($^) DOIIX=$(DOIIX) CCOPTS=$(CCOPTS)"
ifeq ($(DOIIX), true)
	@$(CC) $(CCOPTS) $< 
else
	@$(CC) $(CCOPTS) -c $< -o $@ 
endif

%.ROOT: %.asm
	@echo "Compiling ASM ($@) with ($^) DOIIX=$(DOIIX)"
ifeq ($(DOIIX), true)
	$(CC) $(CCOPTS) $< cc=-DAPPLE2GS
else
	@$(CC) $(CCOPTS) -c $< -o $@
endif

clean:
	@echo "Cleaning"
	$(RM) $(LIBFILENAME) *.o *.sym *.a *.root *.ROOT
	

