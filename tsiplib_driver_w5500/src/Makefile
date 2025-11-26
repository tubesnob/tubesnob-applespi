BINDIR=../bin
TARGET=tsiplib_driver_w5500.o
SRC=tsiplib_w5500_driver.c w5500_raw_socket.c w5500.c
OBJ=$(SRC:.c=.o)

ifeq ($(DOIIX), true)
else
LINKDEPS=/opt/homebrew/Cellar/libftdi/1.5_2/lib/libftdi1.dylib
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

