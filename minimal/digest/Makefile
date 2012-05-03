 #
#The project is the name of the sub project of the washing machine.
#
PROJECT=digest

# Standard stuff.
#
OCFAMODULE_TOP_DIR=../..
include ../../global.makeinfo
#
# TODO this variable should be system dependent.
DBLIBS=-ldb


OBJFILESLIB=$(BUILDDIR)/digest.lo 

EXEC=digest

$(PROJECT):  $(OBJFILESLIB)
	$(LIBTOOL) --mode=link g++ -o $(BUILDDIR)/$(EXEC) $(DBLIBS) $(OBJFILESLIB) $(OCFALIBS)

install: all
	install -d $(INSTALL_SBIN)
	install -d $(INSTALL_STATIC)
	install -d $(INSTALL_STATIC)/hashsets
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(PROJECT) $(INSTALL_SBIN)
	chgrp ocfa $(INSTALL_SBIN)/$(PROJECT)
	chmod g+s $(INSTALL_SBIN)/$(PROJECT)
	cp -u static/*db $(INSTALL_STATIC)/hashsets
	chgrp ocfa $(INSTALL_STATIC)/hashsets/*
	chmod 660 $(INSTALL_STATIC)/hashsets/*
