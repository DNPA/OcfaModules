#
# Automatically generated Makefile by m4 module script.
# You may edit this.
#
PROJECT=fivesfaces

# Standard stuff.
#
OCFAMODULE_TOP_DIR=../..
include ../../global.makeinfo

OBJFILESLIB=$(BUILDDIR)/fivesfaces.lo 
EXEC=fivesfaces

$(PROJECT):  $(OBJFILESLIB)
	$(LIBTOOL) --mode=link  g++ -o $(BUILDDIR)/$(EXEC) $(OBJFILESLIB) $(OCFALIBS) -lcv -lhighgui -lcvaux -lcxcore

install: all
	if [ ! -d $(INSTALL_SBIN) ]; then mkdir $(INSTALL_SBIN); fi
	if [ -d $(INSTALL_ETC) ];then \
		cp src/haarcascade_frontalface_default.xml $(INSTALL_ETC);\
	fi;
 
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(PROJECT) $(INSTALL_SBIN)
	chgrp ocfa $(INSTALL_SBIN)/$(PROJECT)
	chmod g+s $(INSTALL_SBIN)/$(PROJECT)

