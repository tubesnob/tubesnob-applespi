include Makefile.env.include

all:
	$(MAKE) -C tslib/SRC $@ DOIIX=$(DOIIX)
	$(MAKE) -C tsspilib_driver_ftdi/SRC $@ DOIIX=$(DOIIX)
	$(MAKE) -C tsspilib_driver_a2gpio/SRC $@ DOIIX=$(DOIIX)
	$(MAKE) -C tsspilib/SRC $@ DOIIX=$(DOIIX)
	$(MAKE) -C tssocketlib/SRC $@ DOIIX=$(DOIIX)
	$(MAKE) -C tslib_test/SRC $@ DOIIX=$(DOIIX)
	$(MAKE) -C tsspilib_test/SRC $@ DOIIX=$(DOIIX)
	$(MAKE) -C tssocketlib_test/SRC $@ DOIIX=$(DOIIX)
#	$(MAKE) -C spidl/SRC $@ DOIIX=$(DOIIX)
#	$(MAKE) -C spiws/SRC $@ DOIIX=$(DOIIX)

clean: 
	$(MAKE) -C tslib/SRC $@ DOIIX=$(DOIIX)
	$(MAKE) -C tsspilib_driver_ftdi/SRC $@ DOIIX=$(DOIIX)
	$(MAKE) -C tsspilib_driver_a2gpio/SRC $@ DOIIX=$(DOIIX)
	$(MAKE) -C tsspilib/SRC $@ DOIIX=$(DOIIX)
	$(MAKE) -C tssocketlib/SRC $@ DOIIX=$(DOIIX)
	$(MAKE) -C tslib_test/SRC $@ DOIIX=$(DOIIX)
	$(MAKE) -C tsspilib_test/SRC $@ DOIIX=$(DOIIX)
	$(MAKE) -C tssocketlib_test/SRC $@ DOIIX=$(DOIIX)
#	$(MAKE) -C spidl/SRC $@ DOIIX=$(DOIIX)
#	$(MAKE) -C spiws/SRC $@ DOIIX=$(DOIIX)

include Makefile.env.debug



