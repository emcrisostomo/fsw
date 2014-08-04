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
libfsw_la_SOURCES  = c/libfsw.cpp
libfsw_la_SOURCES += c/libfsw_log.cpp c/libfsw_log.h
libfsw_la_SOURCES += c++/libfsw_map.h c++/libfsw_set.h
libfsw_la_SOURCES += c++/libfsw_exception.cpp c++/libfsw_exception.h
libfsw_la_SOURCES += c++/event.cpp c++/event.h
libfsw_la_SOURCES += c++/filter.h
libfsw_la_SOURCES += c++/monitor.cpp c++/monitor.h
libfsw_la_SOURCES += c++/fsevent_monitor.cpp c++/fsevent_monitor.h
libfsw_la_SOURCES += c++/kqueue_monitor.cpp c++/kqueue_monitor.h
libfsw_la_SOURCES += c++/poll_monitor.cpp c++/poll_monitor.h
libfsw_la_SOURCES += c++/inotify_monitor.cpp c++/inotify_monitor.h
libfsw_la_SOURCES += c++/path_utils.cpp c++/path_utils.h

libfsw_cdir = $(pkgincludedir)/c
libfsw_c_HEADERS = c/libfsw.h c/cevent.h c/cfilter.h c/cmonitor.h

libfsw_cppdir = $(pkgincludedir)/c++
libfsw_cpp_HEADERS  = c++/monitor.h c++/fsevent_monitor.h c++/kqueue_monitor.h
libfsw_cpp_HEADERS += c++/poll_monitor.h c++/inotify_monitor.h
