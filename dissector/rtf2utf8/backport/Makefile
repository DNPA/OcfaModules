

VERSION=0.3
PREFIX=/usr/local/ocfa$(VERSION)
INSTALLIB=/usr/lib
PROJECT=rtf2utf8
SRCDIR=src
BUILDDIR=build
INCDIR=inc -I/usr/include/pgsql/ -I../../../ipc/arch/klpd/inc -I/usr/local/pgsql/include/ -I../../../ipc/inc -I.
LIBNAME=$(PROJECT)
LIBFILE=lib$(LIBNAME).so
LIBS=
GCCFLAGS=-Wall -pedantic-errors -Wno-long-long -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
all: rtf2utf8 processguardian
$(BUILDDIR)/%.lo: $(SRCDIR)/%.cpp
	if [ ! -d $(BUILDDIR) ]; then mkdir $(BUILDDIR);fi
	libtool g++ -g -c $(GCCFLAGS) -I$(INCDIR) -o $@ $<

rtf2utf8: ../../../ipc/build/libocfa.la $(BUILDDIR)/Rtf2Utf8.lo
	libtool g++ -g $(LIBS) -o $(BUILDDIR)/rtf2utf8 ../../../ipc/build/libocfa.la $(BUILDDIR)/Rtf2Utf8.lo


processguardian: $(SRCDIR)/ProcessGuardian.java
	javac -d $(BUILDDIR) $(SRCDIR)/ProcessGuardian.java

clean:
	if [ -d doc/generated ]; then rm -r doc/generated; fi
	libtool rm  $(BUILDDIR)/*.lo

install: all
#	libtool cp $(BUILDDIR)/lib$(PROJECT).la $(INSTALLIB)
	cp $(BUILDDIR)/ProcessGuardian.class $(OCFAROOT)/tools
uninstall:
	rm -r $(INSTALLLIB)
