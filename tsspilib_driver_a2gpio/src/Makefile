BINDIR=../bin
TARGET=tsspilib_driver_a2gpio.o

ifeq ($(DOIIX), true)
SRC=tsspilib_a2gpio_main.c tsspilib_a2gpio_spireadb.asm tsspilib_a2gpio_spisendb.asm
OBJ=tsspilib_a2gpio_spireadb.ROOT tsspilib_a2gpio_spisendb.ROOT tsspilib_a2gpio_main.o 
else
SRC=tsspilib_a2gpio_dummy.c
OBJ=tsspilib_a2gpio_dummy.o
endif 

include ../../Makefile.env.include

all: clean $(TARGET)

$(TARGET): dirs $(OBJ)
ifeq ($(DOIIX), true)
		$(eval TRX=$(addprefix +,$(OBJ)))
		$(eval TRX2=$(TRX:.o=.a))
		$(MAKELIB) $(MAKELIBOPTS) $(BINDIR)/$(TARGET) $(TRX2)
else
		@echo Nothing to do
		@#$(MAKELIB) $(MAKELIBOPTS) $(BINDIR)/$(TARGET) $(OBJ)
endif

%.o: %.c
	@echo "Compiling C ($@) with ($^) DOIIX=$(DOIIX) CCOPTS=$(CCOPTS)"
ifeq ($(DOIIX), true)
	$(CC) $(CCOPTS) $< 
else
	$(CC) $(CCOPTS) -c $< -o $@ 
endif

%.ROOT: %.asm
	@echo "Compiling ASM ($@) with ($^) DOIIX=$(DOIIX)"
ifeq ($(DOIIX), true)
	$(CC) $(CCOPTS) $< cc=-DAPPLE2GS
else
	$(CC) $(CCOPTS) -c $< -o $@
endif

clean:
	@echo "Cleaning"
	@$(RM) -dR $(BINDIR) $(TARGET) *.dSYM *.o *.o.a *.o.ROOT *.a *.root *.sym *.ROOT

$(BINDIR):
	@mkdir $(BINDIR)

dirs: $(BINDIR) 

