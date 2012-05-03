OCFAMODULE_TOP_DIR=../..
PROJECT=pkr
include ../../global.makeinfo
SHELL=/bin/sh
INSTALL=install

OBJFILESLIB=$(BUILDDIR)/Pkr.lo
GCCFLAGS+= -Wno-deprecated
EXEC=$(PROJECT)

$(PROJECT):  $(OBJFILESLIB)
	$(LIBTOOL) --mode=link g++ -o $(BUILDDIR)/$(EXEC) $(OBJFILESLIB) $(OCFALIBS)

install: all
	if [ ! -d $(INSTALL_SBIN) ]; then mkdir $(INSTALL_SBIN); fi
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(PROJECT) $(INSTALL_SBIN)
	chgrp ocfa $(INSTALL_SBIN)/$(PROJECT)
	chmod g+s $(INSTALL_SBIN)/$(PROJECT)
