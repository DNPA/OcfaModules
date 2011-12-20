#
#The project is the name of the sub project of the washing machine.
#
PROJECT=antiword

# Standard stuff.
#
OCFAMODULE_TOP_DIR=../..
include ../../global.makeinfo

OBJFILESLIB=$(BUILDDIR)/antiword.lo 


#Directory in which all test code is located.

# the lib file contains the name of the library that should be created.
EXEC=antiword


$(PROJECT):  $(OBJFILESLIB)
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/$(EXEC) $(OBJFILESLIB) $(OCFALIBS)

install: all
	if [ ! -d $(INSTALL_SBIN) ]; then mkdir $(INSTALL_SBIN); fi
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(PROJECT) $(INSTALL_SBIN)
	chgrp ocfa $(INSTALL_SBIN)/$(PROJECT)
	chmod g+s $(INSTALL_SBIN)/$(PROJECT)

