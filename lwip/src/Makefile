include Filelists.mk
LWIPDIR=.
#LWIPBASEFILES=$(LWIPNOAPPSFILES)
LWIPBASEFILES=$(COREFILES) \
	$(CORE4FILES) \
	$(NETIFFILES) \

#$(COREFILES) \
#	$(CORE4FILES) \
#	$(NETIFFILES) \
#	$(LWIPNOAPPSFILES)
	
BINDIR=../bin
TARGET=lwip.o
SRC=$(LWIPBASEFILES)
OBJ=$(SRC:.c=.o)

include ../../Makefile.env.include

all: clean $(TARGET)
	@echo $(LWIPBASEFILES)
	
$(TARGET): dirs $(OBJ)
		@echo "Linking ($@) with ($^) DOIIX=$(DOIIX)"
ifeq ($(DOIIX), true)
		$(eval TRX=$(addprefix +,$(OBJ)))
		$(eval TRX2=$(TRX:.o=.a))
		$(MAKELIB) $(MAKELIBOPTS) $(BINDIR)/$(TARGET) $(TRX2)
else
		#libtool -o $(BINDIR)/$(TARGET) $(OBJ)
		$(MAKELIB) $(MAKELIBOPTS) $(BINDIR)/$(TARGET) $(OBJ)
endif

%.o: %.c
	@echo "Compiling C ($@) with ($^) DOIIX=$(DOIIX)"
ifeq ($(DOIIX), true)
	$(eval TRX=$(subst .o,,$@))
	$(CC) $(CCOPTS) $< keep=$(TRX) cc=-i"$(LWIPDIR)/include/" cc=-i"$(LWIPDIR)/"
else
	$(CC) $(CCOPTS) -c $< -o $@ -I $(LWIPDIR)/include/ -I $(LWIPDIR)/
endif

clean:
	@echo "Cleaning"
	@$(RM) -dR $(BINDIR) $(TARGET) $(OBJ) *.dSYM *.o *.o.a *.o.ROOT *.a *.root *.sym *.ROOT *.a.a

$(BINDIR):
	@mkdir $(BINDIR)

dirs: $(BINDIR) 

