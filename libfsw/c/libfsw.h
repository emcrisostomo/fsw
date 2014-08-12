#ifndef LIBFSW_H
#  define LIBFSW_H

#  include "cevent.h"
#  include "cmonitor.h"
#  include "cfilter.h"
#  include "error.h"

#  ifdef __cplusplus
extern "C"
{
#  endif

  typedef unsigned int FSW_HANDLE;

#  define FSW_INVALID_HANDLE -1

#  if defined(HAVE_CXX_THREAD_LOCAL)
#    define FSW_THREAD_LOCAL thread_local
#  endif

  FSW_HANDLE fsw_init_session(const fsw_monitor_type type = system_default_monitor_type);
  int fsw_add_path(const FSW_HANDLE handle, const char * path);
  int fsw_set_callback(const FSW_HANDLE handle,
                       const FSW_CEVENT_CALLBACK callback);
  int fsw_set_latency(const FSW_HANDLE handle, const double latency);
  int fsw_set_recursive(const FSW_HANDLE handle, const bool recursive);
  int fsw_set_follow_symlinks(const FSW_HANDLE handle,
                              const bool follow_symlinks);
  int fsw_add_filter(const FSW_HANDLE handle, const fsw_cmonitor_filter filter);
  int fsw_run_monitor(const FSW_HANDLE handle);
  int fsw_monitor_join(const FSW_HANDLE handle);
  int fsw_destroy_session(const FSW_HANDLE handle);
  int fsw_set_last_error(const int error);
  int fsw_last_error();
  // TODO: implement function to signal a monitor to stop.
  bool fsw_is_verbose();

#  ifdef __cplusplus
}
#  endif

#endif /* LIBFSW_H */