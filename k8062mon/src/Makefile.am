## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = k8062mon

k8062mon_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	k8062.cpp k8062.h \
	channels.cpp channels.h \
	settings.cpp settings.h \
	ui.cpp


k8062mon_LDADD = @PACKAGE_LIBS@ $(INTLLIBS)

