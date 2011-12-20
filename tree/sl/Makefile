#
#The project is the name of the sub project of the washing machine.
#
LIBTOOL=libtool
PROJECT=libsl
OCFAMODULE_TOP_DIR=../..
INTERFACEDIR=/usr/local/digiwash
include ../../global.makeinfo
include ../../RELEASE.makeinfo
VERSION=5:6:0


OBJFILESLIB=$(BUILDDIR)/constructor.lo $(BUILDDIR)/SoftLinkFactory.lo $(BUILDDIR)/SoftLinkNode.lo 


$(PROJECT): $(OBJFILESLIB)
	$(LIBTOOL) --tag=CPP --tag=CPP  --mode=link $(CPP) ${MISCLIB}  ${TREEGRAPH} -rpath $(INSTALLIB) -release $(RELEASE) -version-info $(VERSION) $(LTLINKFLAGS) -o $(BUILDDIR)/$(PROJECT).la $(OBJFILESLIB) ${INCFLAGS} -lcarvpathrepository


install: $(PROJECT)
	install -d $(INSTALLIB)
	$(LIBTOOL) --mode=install cp $(BUILDDIR)/$(PROJECT).la $(INSTALLIB)

