OCFAMODULE_TOP_DIR=../..
PROJECT=file426
include ../../global.makeinfo
SHELL=/bin/sh
INSTALL=install

OBJFILESLIB=$(BUILDDIR)/file426.lo  $(BUILDDIR)/compoundbinaryfile.lo
GCCFLAGS+= -Wno-deprecated
EXEC=file426

#
#Uncomment one of the folowing to select an architecture
$(PROJECT):  /usr/local/digiwash/inc/magic.h $(OBJFILESLIB) 
	$(LIBTOOL) --mode=link g++ -o $(BUILDDIR)/$(EXEC) $(OBJFILESLIB) $(OCFALIBS) -L/usr/local/digiwash/lib -lmagic


/usr/local/digiwash/inc/magic.h:
	./build_libmagic.sh

install: all
	if [ ! -d $(INSTALL_SBIN) ]; then mkdir $(INSTALL_SBIN); fi
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(PROJECT) $(INSTALL_SBIN)
	chgrp ocfa $(INSTALL_SBIN)/$(PROJECT)
	chmod g+s $(INSTALL_SBIN)/$(PROJECT)
	ln -sf $(DISTDIR)/sbin/file426 $(DISTDIR)/sbin/file
#	chgrp ocfa $(INSTALL_ETC)/magic*
#	chmod g+s $(INSTALL_ETC)/magic*
