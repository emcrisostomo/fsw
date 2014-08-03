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
#  define FSW_ERR_SESSION_UNKNOWN         (1 << 0)
#  define FSW_ERR_MONITOR_ALREADY_EXISTS  (1 << 1)
#  define FSW_ERR_MEMORY                  (1 << 2)
#  define FSW_ERR_UNKNOWN_MONITOR_TYPE    (1 << 3)
#  define FSW_ERR_CALLBACK_NOT_SET        (1 << 4)
#  define FSW_ERR_PATHS_NOT_SET           (1 << 5)
#  define FSW_ERR_UNKNOWN_MONITOR         (1 << 6)
#  define FSW_ERR_MISSING_CONTEXT         (1 << 7)
#  define FSW_ERR_INVALID_PATH            (1 << 8)
#  define FSW_ERR_INVALID_CALLBACK        (1 << 9)
#  define FSW_ERR_INVALID_LATENCY         (1 << 10)

  typedef unsigned int FSW_HANDLE;

#  define FSW_INVALID_HANDLE -1

#  if defined(HAVE_CXX_THREAD_LOCAL)
#    define FSW_THREAD_LOCAL thread_local
#  else
#    define FSW_THREAD_LOCAL
#  endif

  FSW_HANDLE fsw_init_session(const fsw_monitor_type type = fsw_monitor_type::system_default);
  void fsw_add_path(const FSW_HANDLE handle, const char * path);
  void fsw_set_callback(const FSW_HANDLE handle,
                        const CEVENT_CALLBACK callback);
  void fsw_set_latency(const FSW_HANDLE handle, const double latency);
  void fsw_set_recursive(const FSW_HANDLE handle, const bool recursive);
  void fsw_set_follow_symlinks(const FSW_HANDLE handle,
                               const bool follow_symlinks);
  void fsw_add_filter(const FSW_HANDLE handle, const cmonitor_filter filter);
  unsigned int fsw_last_error();
  bool fsw_is_verbose();
  void fsw_set_verbose(FSW_HANDLE handle, bool verbose);

#  ifdef __cplusplus
}
#  endif

#endif /* LIBFSW_H */