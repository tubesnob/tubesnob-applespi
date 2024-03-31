LIBFILENAME=tslib.a
SRC=getopt.c nucrc16.c tslist.c tsmem.c utils.c ini.c tsarray.c tslog.c tsstring.c
OBJ=$(SRC:.c=.o)
DOIIX=false

macos: macos_vars all

macos_vars:
	$(eval MAKELIB=libtool)
	$(eval MAKELIBOPTS=-static -o )
	$(eval CC=clang)
	$(eval CCOPTS= -c -D__MACOS__)

apple2gs: apple2gs_vars all

apple2gs_vars:
	$(eval MAKELIB=iix makelib)
	$(eval MAKELIBOPTS=)
	$(eval CC=iix compile)
	$(eval CCOPTS=+O -D -P )
	
dump:
	@echo "MAKELIB=$(MAKELIB)"
	@echo "MAKELIBOPTS=$(MAKELIBOPTS)"
	@echo "CC=$(CC)"
	@echo "CCOPTS=$(CCOPTS)"
	@echo "DOIIX=$(DOIIX)"

all: clean dump $(LIBFILENAME)

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
	@echo "Compiling ($@) with ($^) DOIIX=$(DOIIX)"
ifeq ($(DOIIX), true)
	@$(CC) $(CCOPTS) $<
else
	@$(CC) $(CCOPTS) -o $@ $<
endif

debug:
	@echo "obj: $(OBJ)"

clean:
	@echo "Cleaning"
	$(RM) $(LIBFILENAME) $(OBJ) $(OBJ:.o=.sym) $(OBJ:.o=.a) $(OBJ:.o=.root)
	

