 #
#The project is the name of the sub project of the washing machine.
#
PROJECT=outlook

# Standard stuff.
#
OCFAMODULE_TOP_DIR=../..
include ../../global.makeinfo

OBJFILESLIB=$(BUILDDIR)/outlook.lo 



EXEC=outlook

$(PROJECT):  $(OBJFILESLIB)
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/$(EXEC) $(OBJFILESLIB) $(OCFALIBS)


install: all
	install -d $(INSTALL_SBIN)
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(PROJECT) $(INSTALL_SBIN)
	chgrp ocfa $(INSTALL_SBIN)/$(PROJECT)
	chmod g+s $(INSTALL_SBIN)/$(PROJECT)

uninstall:
	rm -r $(INSTALL_SBIN)/$(PROJECT)
	rm -r $(HTML_DOCDIR)
			
