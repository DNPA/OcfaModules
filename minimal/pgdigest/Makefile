 #
#The project is the name of the sub project of the washing machine.
#
PROJECT=pgdigest

# Standard stuff.
#
OCFAMODULE_TOP_DIR=../..
include ../../global.makeinfo
#
# TODO this variable should be system dependent.
DBLIBS=-lpq


OBJFILESLIB=$(BUILDDIR)/pgdigest.lo 

EXEC=pgdigest

$(PROJECT):  $(OBJFILESLIB)
	$(LIBTOOL) --mode=link g++ -o $(BUILDDIR)/$(EXEC) $(DBLIBS) $(OBJFILESLIB) $(OCFALIBS)

install: all
	install -d $(INSTALL_SBIN)
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(PROJECT) $(INSTALL_SBIN)
	chgrp ocfa $(INSTALL_SBIN)/$(PROJECT)
	chmod g+s $(INSTALL_SBIN)/$(PROJECT)
