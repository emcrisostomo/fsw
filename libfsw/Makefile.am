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

lib_LTLIBRARIES = libfsw.la
libfsw_la_SOURCES  = libfsw.cpp
libfsw_la_SOURCES += libfsw_map.h libfsw_set.h
libfsw_la_SOURCES += libfsw_exception.cpp libfsw_exception.h
libfsw_la_SOURCES += event.cpp event.h
libfsw_la_SOURCES += monitor.cpp monitor.h fsw_map.h fsw_set.h
libfsw_la_SOURCES += fsevent_monitor.cpp fsevent_monitor.h
libfsw_la_SOURCES += kqueue_monitor.cpp kqueue_monitor.h
libfsw_la_SOURCES += poll_monitor.cpp poll_monitor.h
libfsw_la_SOURCES += inotify_monitor.cpp inotify_monitor.h
libfsw_la_SOURCES += path_utils.cpp path_utils.h

pkginclude_HEADERS = libfsw.h
