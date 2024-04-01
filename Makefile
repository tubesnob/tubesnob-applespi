include Makefile.env.include

all: 
	$(MAKE) -C tslib/SRC $@ DOIIX=$(DOIIX)
	$(MAKE) -C tslib_test/SRC $@ DOIIX=$(DOIIX)
	

clean: 
	$(MAKE) -C tslib/SRC $@ DOIIX=$(DOIIX)
	$(MAKE) -C tslib_test/SRC $@ DOIIX=$(DOIIX)
	
include Makefile.env.debug



