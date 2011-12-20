#
#The project is the name of the sub project of the washing machine.
#
PROJECT=bunzip

# Standard stuff.
#
OCFAMODULE_TOP_DIR=../..
include ../../global.makeinfo
EXEC=bunzip
OBJFILESLIB=$(BUILDDIR)/bunzip.lo

$(PROJECT):  $(OBJFILESLIB)
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/$(EXEC) $(OBJFILESLIB) $(OCFALIBS)

install: all
	if [ ! -d $(INSTALL_SBIN) ]; then mkdir $(INSTALL_SBIN); fi
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(PROJECT) $(INSTALL_SBIN)/bzip
	chgrp ocfa $(INSTALL_SBIN)/bzip
	chmod g+s $(INSTALL_SBIN)/bzip
