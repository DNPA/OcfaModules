

VERSION=0.3
PREFIX=/usr/local/ocfa$(VERSION)
INSTALLIB=/usr/lib
PROJECT=utf82ucs2
SRCDIR=src
BUILDDIR=build
INCDIR=inc -I/usr/include/pgsql/ -I../../../ipc/arch/klpd/inc -I/usr/local/pgsql/include/ -I../../../ipc/inc -I.
LIBNAME=$(PROJECT)
LIBFILE=lib$(LIBNAME).so
LIBS=
GCCFLAGS=-Wall -pedantic-errors -Wno-long-long -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE
all: utf82ucs2
$(BUILDDIR)/%.lo: $(SRCDIR)/%.cpp
	if [ ! -d $(BUILDDIR) ]; then mkdir $(BUILDDIR);fi
	libtool --tag=CPP g++ -g -c $(GCCFLAGS) -I$(INCDIR) -o $@ $<

utf82ucs2: ../../../ipc/build/libocfa.la $(BUILDDIR)/Utf82Ucs2.lo
	libtool --tag=CPP g++ -g $(LIBS) -o $(BUILDDIR)/utf82ucs2 ../../../ipc/build/libocfa.la $(BUILDDIR)/Utf82Ucs2.lo

clean:
	if [ -d doc/generated ]; then rm -r doc/generated; fi
	libtool rm  $(BUILDDIR)/*.lo

install: all
	libtool cp $(BUILDDIR)/lib$(PROJECT).la $(INSTALLIB)

uninstall:
	rm -r $(INSTALLLIB)
