OCFAMODULE_TOP_DIR=../..
PROJECT=luceneindexer
include ../../global.makeinfo
SHELL=/bin/sh
INSTALL=install

ANALYZEOBJFILES=$(BUILDDIR)/OcfaLengthFilter.lo $(BUILDDIR)/WashAnalyzer.lo
OBJFILESLIB=$(BUILDDIR)/LuceneIndexer.lo $(ANALYZEOBJFILES)
GCCFLAGS+= -I/usr/local/lib/ -I /usr/lib -I /usr/local/lib64 -I /usr/lib64  -I/usr/local/include -g
EXEC=$(PROJECT)

#
#Uncomment one of the folowing to select an architecture
$(PROJECT):  $(OBJFILESLIB)
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/$(EXEC) $(OBJFILESLIB) $(OCFALIBS) -lclucene -L/usr/local/lib

install: all
	if [ ! -d $(INSTALL_SBIN) ]; then mkdir $(INSTALL_SBIN); fi
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(PROJECT) $(INSTALL_SBIN)
	chgrp ocfa $(INSTALL_SBIN)/$(PROJECT)
	chmod g+s $(INSTALL_SBIN)/$(PROJECT)
	ln -sf $(DISTDIR)/sbin/luceneindexer $(DISTDIR)/sbin/indexer

test: $(BUILDDIR)/TestTokenizer.lo $(ANALYZEOBJFILES)
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/testtokenize $(BUILDDIR)/TestTokenizer.lo $(ANALYZEOBJFILES) -lclucene -L/usr/local/lib

testfile: $(BUILDDIR)/TestFileReader.lo 
	$(LIBTOOL) --tag=CPP --mode=link g++ -o $(BUILDDIR)/testfileread $(BUILDDIR)/TestFileReader.lo -lclucene -L/usr/local/lib
