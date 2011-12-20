#
#The project is the name of the sub project of the washing machine.
#
PROJECT=strings

# Standard stuff.
#
OCFAMODULE_TOP_DIR=../..
include ../../global.makeinfo
#
# version number this is used to create some directoreis
#
VERSION=0.5

#
# The main  installation directory in which all stuff will be put.
# shared library files will be put in subdir lib, executables in subdir lib
# and executables that should be executed from other executables in libexec
# according to GNU standards this should be called prefix 
#

OBJFILESLIB=$(BUILDDIR)/Strings.lo 


#Directory in which all test code is located.


# the lib file contains the name of the library that should be created.
LIBNAME=$(PROJECT)
EXEC=strings

# the libs which should be linked with our object files
# 
#LIBS=-ldb $(INSTALLIB)/libocfafs.la $(INSTALLIB)/libocfaevidence.la $(INSTALLIB)/libUnixFileSystem.la $(INSTALLIB)/libocfaevent.la $(INSTALLIB)/libocfamodule.la $(INSTALLIB)/libocfastore.la $(INSTALLIB)/libocfafacade.la $(INSTALLIB)/libocfamessage.la $(INSTALLIB)/libocfamisc.la
$(PROJECT):  $(OBJFILESLIB)
		$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/$(EXEC) $(OBJFILESLIB) $(OCFALIBS)

install: all
	if [ ! -d $(INSTALL_SBIN) ]; then mkdir $(INSTALL_SBIN); fi
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(PROJECT) $(INSTALL_SBIN)
	chgrp ocfa $(INSTALL_SBIN)/$(PROJECT)
	chmod g+s $(INSTALL_SBIN)/$(PROJECT)
