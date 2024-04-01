LIBFILENAME=tsspilib.o

ifeq ($(DOIIX), true)
SRC=tsspilib_main.c tsspilib_asm_spireadb.asm tsspilib_asm_spisendb.asm
OBJ=tsspilib_main.o tsspilib_asm_spireadb.ROOT tsspilib_asm_spisendb.ROOT
else
SRC=tsspilib_main.c tsspilib_readsendunix.c
OBJ=tsspilib_main.o tsspilib_readsendunix.o
endif 

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
	@echo "Compiling C ($@) with ($^) DOIIX=$(DOIIX)"
ifeq ($(DOIIX), true)
	@$(CC) $(CCOPTS) $<
else
	@$(CC) $(CCOPTS) -c $< -o $@
endif

%.ROOT: %.asm
	@echo "Compiling ASM ($@) with ($^) DOIIX=$(DOIIX)"
ifeq ($(DOIIX), true)
	@$(CC) $(CCOPTS) $<
else
	@$(CC) $(CCOPTS) -c $< -o $@
endif

clean:
	@echo "Cleaning"
	$(RM) $(LIBFILENAME) $(OBJ) $(OBJ:.o=.sym) $(OBJ:.o=.a) $(OBJ:.o=.root) $(OBJ:.o=.o.a) $(OBJ:.o=.o.root) *.ROOT
	

