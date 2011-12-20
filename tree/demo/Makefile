#
#The project is the name of the sub project of the washing machine.
#
LIBTOOL=libtool
PROJECT=libdemo
OCFAMODULE_TOP_DIR=../..
INTERFACEDIR=/usr/local/digiwash
#LIBS=-locfamisc -locfatreegraph 
include ../../RELEASE.makeinfo
include ../../global.makeinfo
VERSION=2:0:0


OBJFILESLIB=$(BUILDDIR)/constructor.lo $(BUILDDIR)/DemoFactory.lo $(BUILDDIR)/DemoTopNode.lo $(BUILDDIR)/DemoSubNode.lo 

# Target which creates an shared library. You will always need a
# library because you want to run your regression tests. 
#
#

$(PROJECT): $(OBJFILESLIB)
	$(LIBTOOL) --tag=CPP --tag=CPP  --mode=link $(CPP) ${MISCLIB} ${TREEGRAPH} -rpath $(INSTALLIB) -release $(RELEASE) -version-info $(VERSION) $(LTLINKFLAGS) -o $(BUILDDIR)/$(PROJECT).la $(OBJFILESLIB) ${INCFLAGS}


install: $(PROJECT)
	install -d $(INSTALLIB)
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(PROJECT).la $(INSTALLIB)

