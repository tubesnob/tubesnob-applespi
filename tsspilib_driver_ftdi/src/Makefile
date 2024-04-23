BINDIR=../bin
TARGET=tsspilib_driver_ftdi.o

ifeq ($(DOIIX), true)
SRC=tsspilib_ftdi_dummy.c
OBJ=tsspilib_ftdi_dummy.o
else
LINKDEPS=/usr/local/lib/libftdi1.dylib 
SRC=tsspilib_ftdi_main.c
OBJ=tsspilib_ftdi_main.o
endif 

include ../../Makefile.env.include

all: clean $(TARGET)

$(TARGET): dirs $(OBJ)
		@echo "Linking ($@) with ($^) DOIIX=$(DOIIX)"
ifeq ($(DOIIX), true)
		@echo Nothing to do
		@#$(eval TRX=$(addprefix +,$(OBJ)))
		@#$(eval TRX2=$(TRX:.o=.a))
		@#$(MAKELIB) $(MAKELIBOPTS) $(BINDIR)/$(TARGET) $(TRX2)
else
		$(MAKELIB) $(MAKELIBOPTS) $(BINDIR)/$(TARGET) $(OBJ) $(LINKDEPS)
endif

%.o: %.c
	@echo "Compiling C ($@) with ($^) DOIIX=$(DOIIX)"
ifeq ($(DOIIX), true)
	$(CC) $(CCOPTS) $<
else
	$(CC) $(CCOPTS) -c $< -o $@
endif

clean:
	@echo "Cleaning"
	@$(RM) -dR $(BINDIR) $(TARGET) *.dSYM *.o *.o.a *.o.ROOT *.a *.root *.sym *.ROOT

$(BINDIR):
	@mkdir $(BINDIR)

dirs: $(BINDIR) 

