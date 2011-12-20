#
#The project is the name of the sub project of the washing machine.
#
PROJECT=photorec

# Standard stuff.
#
OCFAMODULE_TOP_DIR=../..
include ../../global.makeinfo

OBJFILESLIB=$(BUILDDIR)/photorec.lo 


EXEC=photorec

$(PROJECT):  $(OBJFILESLIB)
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/$(EXEC) $(OBJFILESLIB) $(OCFALIBS)

test:	$(BUILDDIR)/test.lo
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/test $(BUILDDIR)/test.lo

install: all
	install -d $(INSTALL_SBIN)
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(PROJECT) $(INSTALL_SBIN)
	ln -sf $(INSTALL_SBIN)/$(PROJECT) $(INSTALL_SBIN)/carver
	cp scripts/ocfacarv.sh $(INSTALL_SBIN)
	chgrp ocfa $(INSTALL_SBIN)/$(PROJECT)
	chmod g+s $(INSTALL_SBIN)/$(PROJECT)

build/photorec.lo: src/photorec.cpp
	if [ ! -d build ]; then mkdir build ;fi
	$(LIBTOOL) --tag=CPP --mode=compile g++ -ggdb -c $(GCCFLAGS) $(INCFLAGS) -o build/photorec.lo src/photorec.cpp
