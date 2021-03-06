#
# Copyright (C) 2014, Enrico M. Crisostomo
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
ACLOCAL_AMFLAGS = -I m4

# Refresh configuration if libtool dependencies change
LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status libtool

SUBDIRS = libfsw

bin_PROGRAMS = fsw
fsw_SOURCES  = fsw.cpp fsw.h fsw_log.cpp fsw_log.h

# Link fsw against dependent libraries
fsw_LDADD = libfsw/libfsw.la

man_MANS = fsw.7
EXTRA_DIST = $(man_MANS)
dist_doc_DATA  = README.bsd README.freebsd README.gnu-build-system README.md
dist_doc_DATA += README.osx

dist_bin_SCRIPTS = scripts/fswatch-run-bash scripts/fswatch-run-zsh

install-exec-hook: fswatch-remove-links
if HAVE_ZSH
	$(LN_S) $(DESTDIR)$(bindir)/fswatch-run-zsh $(DESTDIR)$(bindir)/fswatch-run
else
if HAVE_BASH
	$(LN_S) $(DESTDIR)$(bindir)/fswatch-run-bash $(DESTDIR)$(bindir)/fswatch-run
else
	@echo "No compatible shell was found for fswatch-run."
endif
endif

uninstall-local: fswatch-remove-links

fswatch-remove-links:
	-rm $(DESTDIR)$(bindir)/fswatch-run

distclean-local: distclean-local-check
.PHONY: distclean-local-check
distclean-local-check:
	-rm -rf libfsw scripts
