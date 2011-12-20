#
#The project is the name of the sub project of the washing machine.
#
LIBTOOL=libtool
PROJECT=libtskfs
OCFAMODULE_TOP_DIR=../..
INTERFACEDIR=/usr/local/digiwash
include ../../global.makeinfo
VERSION=8:6:2
RELEASE=2.3.0rc1


OBJFILESLIB=$(BUILDDIR)/constructor.lo $(BUILDDIR)/TskFsFactory.lo $(BUILDDIR)/TskFsTopDir.lo $(BUILDDIR)/TskFsDirectory.lo $(BUILDDIR)/TskFsDuplicateDirectory.lo $(BUILDDIR)/TskFsInode.lo $(BUILDDIR)/TskFsBaseDataAttribute.lo $(BUILDDIR)/TskFsCarvPathDataAttribute.lo $(BUILDDIR)/TskFsCarvPathFileSlack.lo $(BUILDDIR)/TskFsResidentDataAttribute.lo $(BUILDDIR)/TskFsFileSlack.lo  $(BUILDDIR)/TskFsStreamDataAttribute.lo $(BUILDDIR)/TskFsMultipleFilesystem.lo

# Target which creates an shared library. You will always need a
# library because you want to run your regression tests. 
#
#

$(PROJECT): $(OBJFILESLIB)
	$(LIBTOOL) --tag=CPP --tag=CPP  --mode=link $(CPP) ${MISCLIB} ${TREEGRAPH} -rpath $(INSTALLIB) -release $(RELEASE) -version-info $(VERSION) $(LTLINKFLAGS) -o $(BUILDDIR)/$(PROJECT).la $(OBJFILESLIB) ${INCFLAGS} -ltsk3 -lcarvpath  -L/usr/local/lib


install: $(PROJECT)
	install -d $(INSTALLIB)
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(PROJECT).la $(INSTALLIB)

