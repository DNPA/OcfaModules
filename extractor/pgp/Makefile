OCFAMODULE_TOP_DIR=../..
PROJECT=pgp
include ../../global.makeinfo
SHELL=/bin/sh
INSTALL=install

OBJFILESLIB=$(BUILDDIR)/Pgp.lo
GCCFLAGS+= -Wno-deprecated
EXEC=$(PROJECT)

#
#Uncomment one of the folowing to select an architecture
$(PROJECT):  $(OBJFILESLIB)
	$(LIBTOOL) --mode=link g++ -o $(BUILDDIR)/$(EXEC) $(OBJFILESLIB) $(OCFALIBS)

install: all
	if [ ! -d $(INSTALL_SBIN) ]; then mkdir $(INSTALL_SBIN); fi
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(PROJECT) $(INSTALL_SBIN)
	chgrp ocfa $(INSTALL_SBIN)/$(PROJECT)
	chmod g+s $(INSTALL_SBIN)/$(PROJECT)
