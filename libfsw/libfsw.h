#ifndef LIBFSW_H
#  define LIBFSW_H

#  include "cevent.h"
#  include "cmonitor.h"

#  ifdef __cplusplus
extern "C"
{
#  endif

  // Error codes
#  define FSW_ERR_SESSION_UNKNOWN         1
#  define FSW_ERR_MONITOR_ALREADY_EXISTS  2
#  define FSW_ERR_MEMORY                  4
#  define FSW_ERR_UNKNOWN_MONITOR_TYPE    8
#  define FSW_ERR_CALLBACK_NOT_SET        16
#  define FSW_ERR_PATHS_NOT_SET           32
#  define FSW_ERR_UNKNOWN_MONITOR         64

  typedef unsigned int FSW_HANDLE;

#  define FSW_INVALID_HANDLE -1
  // TODO Implement Autoconf check
#  define FSW_THREAD_LOCAL /* thread_local */

  FSW_HANDLE fsw_init_session(const fsw_monitor_type type = fsw_monitor_type::system_default);
  unsigned int fsw_last_error();
  bool fsw_is_verbose();
  void fsw_set_verbose(FSW_HANDLE handle, bool verbose);

#  ifdef __cplusplus
}
#  endif

#endif /* LIBFSW_H */