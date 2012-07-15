#
# acad - An avrdude distro adapted for the Ale packet
# Copyright (C) 2012 Victor ADĂSCĂLIȚEI <admin@tuscale.ro>
#
# !!! Original avrdude 5.11 Makefile.am notice follows !!!
# Copyright (C) 2003, 2004  Theodore A. Roth  <troth@openavr.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

#
# $Id: Makefile.am 987 2011-08-26 20:22:09Z joerg_wunsch $
#

EXTRA_DIST   = \
	acad.1 \
	acad.spec \
	bootstrap

CLEANFILES = \
	config_gram.c \
	config_gram.h \
	lexer.c

BUILT_SOURCES = $(CLEANFILES)

#SUBDIRS      = doc @WINDOWS_DIRS@
#DIST_SUBDIRS = doc windows

SUBDIRS = @SUBDIRS_AC@
SUBDIRS += @WINDOWS_DIRS@
DIST_SUBDIRS = @DIST_SUBDIRS_AC@

AM_YFLAGS    = -d

acad_CPPFLAGS = -DCONFIG_DIR=\"$(sysconfdir)\"

libacad_a_CPPFLAGS = -DCONFIG_DIR=\"$(sysconfdir)\"

acad_CFLAGS   = @ENABLE_WARNINGS@

libacad_a_CFLAGS   = @ENABLE_WARNINGS@

acad_LDADD  = $(top_builddir)/$(noinst_LIBRARIES) @LIBUSB@ @LIBUSB_1_0@ @LIBFTDI@ @LIBHID@ -lm

bin_PROGRAMS = acad

noinst_LIBRARIES = libacad.a

# automake thinks these generated files should be in the distribution,
# but this might cause trouble for some users, so we rather don't want
# to have them there.
#
# See
#
# https://savannah.nongnu.org/bugs/index.php?func=detailitem&item_id=15536
#
# for why we don't want to have them.
dist-hook:
	rm -f \
	$(distdir)/lexer.c \
	$(distdir)/config_gram.c \
	$(distdir)/config_gram.h

libacad_a_SOURCES = \
	config_gram.y \
	lexer.l \
	avr.c \
	avr.h \
	avrpart.c \
	avrpart.h \
	config.c \
	config.h \
	confwin.c \
	confwin.h \
	crc16.c \
	crc16.h \
	fileio.c \
	fileio.h \
	lists.c \
	lists.h \
	my_ddk_hidsdi.h \
	pgm.c \
	pgm.h \
	pindefs.h \
	safemode.c \
	safemode.h \
	serial.h \
	tpi.h \
	usbdevs.h \
	usb_libusb.c \
	usbtiny.h \
	usbtiny.c \
	update.h \
	update.c

acad_SOURCES = main.c

man_MANS = acad.1

sysconf_DATA = acad.conf

install-exec-local: backup-acad-conf

distclean-local:
	rm -f acad.conf

# This will get run before the config file is installed.
backup-acad-conf:
	@echo "Backing up acad.conf in ${DESTDIR}${sysconfdir}"
	@if test -e ${DESTDIR}${sysconfdir}/acad.conf; then \
		cp -pR ${DESTDIR}${sysconfdir}/acad.conf \
			${DESTDIR}${sysconfdir}/acad.conf.bak; \
	fi
