include ../../configure.makeinfo
all:
	if [ $(CONF_OK) ]; then \
		echo Configuration seems ok; \
	else \
		echo Can not build with bad configuration;\
	exit 1;\
	fi;
	ant -buildfile ant/build.xml

install: all
	ant -buildfile ant/build.xml deploy
	if ( ! grep -q ole2txt /usr/local/digiwash/etc/javamodules.example ) then echo "ole2txt nl.klpd.tde.ocfamodule.ole2txt.Ole2TxtModule" >> /usr/local/digiwash/etc/javamodules.example ; fi

clean:
	ant -buildfile ant/build.xml clean
