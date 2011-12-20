#
#The project is the name of the sub project of the washing machine.
#
PROJECT=tcpflow

# Standard stuff.
#
OCFAMODULE_TOP_DIR=../..
include ../../global.makeinfo

OBJFILESLIB=$(BUILDDIR)/tcpflow.lo $(BUILDDIR)/datalink.lo $(BUILDDIR)/flow.lo  $(BUILDDIR)/main.lo $(BUILDDIR)/tcpip.lo  $(BUILDDIR)/util.lo $(BUILDDIR)/TcpFlowTreeWalker.lo 

OCFALIBS+= -lpcap 

#Directory in which all test code is located.

# the lib file contains the name of the library that should be created.
EXEC=tcpflow


$(PROJECT):  $(OBJFILESLIB)
	libtool g++ -DHAVE_CONFIG_H -o $(BUILDDIR)/$(EXEC) $(OBJFILESLIB) $(OCFALIBS)

$(BUILDDIR)/%.lo: $(SRCDIR)/%.c
	if [ ! -d $(BUILDDIR) ]; then mkdir $(BUILDDIR);fi
	$(LIBTOOL) --tag=CPP --mode=compile gcc -c  $(INCFLAGS) -I$(BOOSTINCDIR) -o $@ $<


install: all
	if [ ! -d $(INSTALL_SBIN) ]; then mkdir $(INSTALL_SBIN); fi
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(PROJECT) $(INSTALL_SBIN)
	chgrp ocfa $(INSTALL_SBIN)/$(PROJECT)
	chmod g+s $(INSTALL_SBIN)/$(PROJECT)
