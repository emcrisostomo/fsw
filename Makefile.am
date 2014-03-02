bin_PROGRAMS = fsw
fsw_SOURCES  = fsw.cpp fsw.h fsw_log.cpp fsw_log.h path_utils.cpp path_utils.h
fsw_SOURCES += fsw_exception.cpp fsw_exception.h event.cpp event.h
fsw_SOURCES += monitor.cpp monitor.h fsw_map.h fsw_set.h
fsw_SOURCES += fsevent_monitor.cpp fsevent_monitor.h
fsw_SOURCES += kqueue_monitor.cpp kqueue_monitor.h
fsw_SOURCES += poll_monitor.cpp poll_monitor.h

if USE_CORESERVICES
  fsw_LDFLAGS = -framework CoreServices
endif

man_MANS = fsw.7
EXTRA_DIST = $(man_MANS)
dist_doc_DATA  = README.bsd README.freebsd README.gnu-build-system README.md
dist_doc_DATA += README.osx
