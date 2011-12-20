 #
#The project is the name of the sub project of the washing machine.
#
PROJECT=mailsplit

# Standard stuff.
#
OCFAMODULE_TOP_DIR=../..
include ../../global.makeinfo

OBJFILESLIB=$(BUILDDIR)/mailsplit.lo 


EXEC=mailsplit

$(PROJECT):  $(OBJFILESLIB)
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/$(EXEC) $(OBJFILESLIB) $(OCFALIBS)

test:	$(BUILDDIR)/test.lo
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/test $(BUILDDIR)/test.lo

install: all
	install -d $(INSTALL_SBIN)
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(PROJECT) $(INSTALL_SBIN)
	cp scripts/mailsplit.pl $(INSTALL_SBIN)
	chgrp ocfa $(INSTALL_SBIN)/$(PROJECT)
	chmod g+s $(INSTALL_SBIN)/$(PROJECT)

