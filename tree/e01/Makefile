#
#The project is the name of the sub project of the washing machine.
#
LIBTOOL=libtool
PROJECT=libe01
OCFAMODULE_TOP_DIR=../..
INTERFACEDIR=/usr/local/digiwash
include ../../global.makeinfo
include ../../RELEASE.makeinfo
VERSION=5:0:0


OBJFILESLIB=$(BUILDDIR)/constructor.lo $(BUILDDIR)/E01Factory.lo $(BUILDDIR)/E01Node.lo 

# Target which creates an shared library. You will always need a
# library because you want to run your regression tests. 
#
#

$(PROJECT): $(OBJFILESLIB)
	$(LIBTOOL) --tag=CPP --tag=CPP  --mode=link $(CPP) ${MISCLIB} ${TREEGRAPH} -rpath $(INSTALLIB) -release $(RELEASE) -version-info $(VERSION) $(LTLINKFLAGS) -o $(BUILDDIR)/$(PROJECT).la $(OBJFILESLIB) ${INCFLAGS} -lewf


install: $(PROJECT)
	install -d $(INSTALLIB)
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(PROJECT).la $(INSTALLIB)

