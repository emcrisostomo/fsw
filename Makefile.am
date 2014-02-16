bin_PROGRAMS = fsw
fsw_SOURCES  = fsw.cpp fsw.h fsw_log.cpp fsw_log.h watcher.cpp watcher.h
fsw_SOURCES += fsevent_watcher.cpp fsevent_watcher.h
fsw_SOURCES += kqueue_watcher.cpp kqueue_watcher.h
fsw_LDFLAGS = -framework CoreServices
man_MANS = fsw.7
EXTRA_DIST = $(man_MANS)
