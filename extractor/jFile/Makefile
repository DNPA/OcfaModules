OCFAMODULE_TOP_DIR=../..
PROJECT=jFile
include ../../global.makeinfo
SHELL=/bin/sh
INSTALL=install

OBJFILESLIB=$(BUILDDIR)/jFile.lo  $(BUILDDIR)/compoundbinaryfile.lo
GCCFLAGS+= -Wno-deprecated
EXEC=jFile

#
#Uncomment one of the folowing to select an architecture
$(PROJECT):  $(OBJFILESLIB)
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/$(EXEC) $(OBJFILESLIB) -lmagic $(OCFALIBS)

install: all
	if [ ! -d $(INSTALL_SBIN) ]; then mkdir $(INSTALL_SBIN); fi
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(PROJECT) $(INSTALL_SBIN)
	chgrp ocfa $(INSTALL_SBIN)/$(PROJECT)
	chmod g+s $(INSTALL_SBIN)/$(PROJECT)
	if [ -d $(INSTALL_ETC) ];then \
	   ./installmagic.sh $(INSTALL_ETC);\
	fi;
	chgrp ocfa $(INSTALL_ETC)/magic.mime*
	chmod g+s $(INSTALL_ETC)/magic.mime*
