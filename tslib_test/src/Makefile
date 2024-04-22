TARGETFILENAME=tslib_test_main
SRC=tslib_test_main.c
OBJ=$(SRC:.c=.o)
LINKDEPS=../../tslib/src/tslib.o
OUTDIR=bin

include ../../Makefile.env.include

all: clean $(TARGETFILENAME)

$(TARGETFILENAME): dirs $(OBJ)
ifeq ($(DOIIX), true)
		$(eval TRX2=$(OBJ:.o=.a))
		$(LINK) $(LINKOPTS) $(TRX2) $(LINKDEPS) keep=$(OUTDIR)/$(TARGETFILENAME)
else
		#ignore linking phase
endif

%.o: %.c
ifeq ($(DOIIX), true)
	$(CC) $(CCOPTS) $<
else
	$(CC) $(CCOPTS) $< $(LINKDEPS) -o $(OUTDIR)/$(TARGETFILENAME)
endif

clean:
	@echo "Cleaning"
	$(RM) -dR $(OUTDIR) $(TARGETFILENAME) $(OBJ) $(OBJ:.o=.sym) $(OBJ:.o=.a) $(OBJ:.o=.root) $(OBJ:.o=.o.a) $(OBJ:.o=.o.root) *.dSYM

$(OUTDIR):
	mkdir $(OUTDIR)

dirs: $(OUTDIR) 
