LIBFILENAME=tslib.o
SRC=getopt.c nucrc16.c tslist.c tsmem.c utils.c ini.c tsarray.c tslog.c tsstring.c
OBJ=$(SRC:.c=.o)

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
	@echo "Compiling ($@) with ($^) DOIIX=$(DOIIX)"
ifeq ($(DOIIX), true)
	@$(CC) $(CCOPTS) $<
else
	@$(CC) $(CCOPTS) -c $< -o $@
endif

clean:
	@echo "Cleaning"
	$(RM) $(LIBFILENAME) $(OBJ) $(OBJ:.o=.sym) $(OBJ:.o=.a) $(OBJ:.o=.root) $(OBJ:.o=.o.a) $(OBJ:.o=.o.root)
	

