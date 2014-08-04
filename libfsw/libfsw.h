#ifndef LIBFSW_H
#  define LIBFSW_H

#  include "cevent.h"
#  include "cmonitor.h"
#  include "cfilter.h"

#  ifdef __cplusplus
extern "C"
{
#  endif

  // Error codes
#  define FSW_OK                            0
#  define FSW_ERR_SESSION_UNKNOWN           (1 << 0)
#  define FSW_ERR_MONITOR_ALREADY_EXISTS    (1 << 1)
#  define FSW_ERR_MEMORY                    (1 << 2)
#  define FSW_ERR_UNKNOWN_MONITOR_TYPE      (1 << 3)
#  define FSW_ERR_CALLBACK_NOT_SET          (1 << 4)
#  define FSW_ERR_PATHS_NOT_SET             (1 << 5)
#  define FSW_ERR_UNKNOWN_MONITOR           (1 << 6)
#  define FSW_ERR_MISSING_CONTEXT           (1 << 7)
#  define FSW_ERR_INVALID_PATH              (1 << 8)
#  define FSW_ERR_INVALID_CALLBACK          (1 << 9)
#  define FSW_ERR_INVALID_LATENCY           (1 << 10)
#  define FSW_ERR_MONITOR_ALREADY_RUNNING   (1 << 11)
#  define FSW_ERR_STALE_MONITOR_THREAD      (1 << 12)
#  define FSW_ERR_THREAD_FAULT              (1 << 13)

  typedef unsigned int FSW_HANDLE;

#  define FSW_INVALID_HANDLE -1

#  if defined(HAVE_CXX_THREAD_LOCAL)
#    define FSW_THREAD_LOCAL thread_local
#  endif

  FSW_HANDLE fsw_init_session(const fsw_monitor_type type = fsw_monitor_type::system_default);
  int fsw_add_path(const FSW_HANDLE handle, const char * path);
  int fsw_set_callback(const FSW_HANDLE handle,
                       const CEVENT_CALLBACK callback);
  int fsw_set_latency(const FSW_HANDLE handle, const double latency);
  int fsw_set_recursive(const FSW_HANDLE handle, const bool recursive);
  int fsw_set_follow_symlinks(const FSW_HANDLE handle,
                              const bool follow_symlinks);
  int fsw_add_filter(const FSW_HANDLE handle, const cmonitor_filter filter);
  int fsw_run_monitor(const FSW_HANDLE handle);
  int fsw_destroy_session(const FSW_HANDLE handle);
  int fsw_set_last_error(const int error);
#  if defined(HAVE_CXX_THREAD_LOCAL)  
  int fsw_last_error();
  // TODO: implement function to signal a monitor to stop.
#  endif
  bool fsw_is_verbose();

#  ifdef __cplusplus
}
#  endif

#endif /* LIBFSW_H */