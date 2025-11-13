BINDIR=../bin
TARGET=tssocketlib.o
SRC=tssocketlib_main.c tssocketlib_w5500.c
OBJ=$(SRC:.c=.o)

ifeq ($(DOIIX), true)
LINKDEPS=../../tslib/bin/tslib.o ../../tsspilib/bin/tsspilib.o 
else
#LINKDEPS=/usr/local/lib/libftdi1.dylib ../../tslib/bin/tslib.o ../../tsspilib/bin/tsspilib.o 
LINKDEPS=/opt/homebrew/Cellar/libftdi/1.5_2/lib/libftdi1.dylib ../../tslib/bin/tslib.o ../../tsspilib/bin/tsspilib.o 
endif 

include ../../Makefile.env.include

all: clean $(TARGET)

$(TARGET): dirs $(OBJ)
		@echo "Linking ($@) with ($^) DOIIX=$(DOIIX)"
ifeq ($(DOIIX), true)
		@$(eval TRX=$(addprefix +,$(OBJ)))
		@$(eval TRX2=$(TRX:.o=.a))
		$(MAKELIB) $(MAKELIBOPTS) $(BINDIR)/$(TARGET) $(TRX2)
else
		$(MAKELIB) $(MAKELIBOPTS) $(BINDIR)/$(TARGET) $(OBJ) $(LINKDEPS)
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
	@$(RM) -dR $(BINDIR) $(TARGET) *.dSYM *.o *.o.a *.o.ROOT *.a *.root *.sym *.ROOT

$(BINDIR):
	@mkdir $(BINDIR)

dirs: $(BINDIR) 


