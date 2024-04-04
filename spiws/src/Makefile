TARGETFILENAME=spiws
SRC=spiws_main.c spiws_server.c spiws_init_w5500.c spiws_config.c
OBJ=$(SRC:.c=.o)
LINKDEPS=../../tslib/src/tslib.o ../../tssocketlib/src/tssocketlib.o ../../tsspilib/src/tsspilib.o

include ../../Makefile.env.include

all: clean $(TARGETFILENAME)

$(TARGETFILENAME): $(OBJ)
ifeq ($(DOIIX), true)
		$(eval TRX2=$(OBJ:.o=.a))
		$(LINK) $(LINKOPTS) $(TRX2) $(LINKDEPS) keep=$@ 
else
		$(CC) $(OBJ) $(LINKDEPS) -o $(TARGETFILENAME) -v -Xlinker -execute -Xlinker -v -arch x86_64
endif

%.o: %.c
ifeq ($(DOIIX), true)
	$(CC) $(CCOPTS) $<
else
	$(CC) $(CCOPTS) -c $< -arch x86_64
endif

clean:
	@echo "Cleaning"
	$(RM) $(TARGETFILENAME) $(OBJ) $(OBJ:.o=.sym) $(OBJ:.o=.a) $(OBJ:.o=.root) $(OBJ:.o=.o.a) $(OBJ:.o=.o.root)
	

