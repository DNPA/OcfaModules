#
#The project is the name of the sub project of the washing machine.
#
LIBTOOL=libtool
PROJECT=libmmls
OCFAMODULE_TOP_DIR=../..
INTERFACEDIR=/usr/local/digiwash
include ../../global.makeinfo
VERSION=5:4:0
RELEASE=2.3.0rc1


OBJFILESLIB=$(BUILDDIR)/constructor.lo $(BUILDDIR)/MmlsFactory.lo $(BUILDDIR)/MmlsTopNode.lo $(BUILDDIR)/MmlsTable.lo $(BUILDDIR)/CarvPathPartition.lo  $(BUILDDIR)/StreamPartition.lo

# Target which creates an shared library. You will always need a
# library because you want to run your regression tests. 
#
#

$(PROJECT): $(OBJFILESLIB)
	$(LIBTOOL) --tag=CPP --tag=CPP  --mode=link $(CPP) ${MISCLIB} ${TREEGRAPH} -rpath $(INSTALLIB) -release $(RELEASE) -version-info $(VERSION) $(LTLINKFLAGS) -o $(BUILDDIR)/$(PROJECT).la $(OBJFILESLIB) ${INCFLAGS} -ltsk3 -lcarvpath  -L/usr/local/lib


install: $(PROJECT)
	install -d $(INSTALLIB)
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(PROJECT).la $(INSTALLIB)

