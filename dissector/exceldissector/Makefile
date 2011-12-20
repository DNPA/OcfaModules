#
#The project is the name of the sub project of the washing machine.
#
PROJECT=excell

# Standard stuff.
#
OCFAMODULE_TOP_DIR=../..
include ../../global.makeinfo
EXEC=exceldissector
OBJFILESLIB=$(BUILDDIR)/ExcelDissector.lo

$(PROJECT):  $(OBJFILESLIB)
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/$(EXEC) $(OBJFILESLIB) $(OCFALIBS)

install: all
	if [ ! -d $(INSTALL_SBIN) ]; then mkdir $(INSTALL_SBIN); fi
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(EXEC) $(INSTALL_SBIN)
	chgrp ocfa $(INSTALL_SBIN)/$(EXEC)
	chmod g+s $(INSTALL_SBIN)/$(EXEC)
	cp script/exceldissect.pl $(INSTALL_SBIN)
