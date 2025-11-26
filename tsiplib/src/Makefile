	
BINDIR=../bin
TARGET=tsiplib.o
SRC=arp.c dhcp.c ethernet.c icmp.c ip.c tcpip.c udp.c
OBJ=$(SRC:.c=.o)

include ../../Makefile.env.include

all: clean $(TARGET)

$(TARGET): dirs $(OBJ)
		@echo "Linking ($@) with ($^) DOIIX=$(DOIIX)"
ifeq ($(DOIIX), true)
		$(eval TRX=$(addprefix +,$(OBJ)))
		$(eval TRX2=$(TRX:.o=.a))
		$(MAKELIB) $(MAKELIBOPTS) $(BINDIR)/$(TARGET) $(TRX2)
else
		$(MAKELIB) $(MAKELIBOPTS) $(BINDIR)/$(TARGET) $(OBJ)
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

