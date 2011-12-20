 #
#The project is the name of the sub project of the washing machine.
#
PROJECT=selfdigest

# Standard stuff.
#
OCFAMODULE_TOP_DIR=../..
include ../../global.makeinfo
#
# TODO this variable should be system dependent.
DBLIBS=-ldb


OBJFILESLIB=$(BUILDDIR)/selfdigest.lo 

EXEC=selfdigest

$(PROJECT):  $(OBJFILESLIB)
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/$(EXEC) $(DBLIBS) $(OBJFILESLIB) $(OCFALIBS)

install: all
	install -d $(INSTALL_SBIN)
	install -d $(INSTALL_STATIC)
	install -d $(INSTALL_STATIC)/hashsets
	$(LIBTOOL) --tag=CPP  --mode=install cp $(BUILDDIR)/$(PROJECT) $(INSTALL_SBIN)
	chgrp ocfa $(INSTALL_SBIN)/$(PROJECT)
	chmod g+s $(INSTALL_SBIN)/$(PROJECT)
