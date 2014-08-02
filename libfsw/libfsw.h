#ifndef LIBFSW_H
#  define LIBFSW_H

#  include "cevent.h"

#  ifdef __cplusplus
extern "C"
{
#  endif

  // Error codes
#  define FSW_ERR_SESSION_UNKNOWN         1
#  define FSW_ERR_MONITOR_ALREADY_EXISTS  2
#  define FSW_ERR_MEMORY                  4

  typedef unsigned int FSW_HANDLE;

#  define FSW_INVALID_HANDLE -1
  // TODO Implement Autoconf check
#  define FSW_THREAD_LOCAL /* thread_local */

  FSW_HANDLE fsw_init_session();
  unsigned int fsw_last_error();
  bool fsw_is_verbose();
  void fsw_set_verbose(FSW_HANDLE handle, bool verbose);

#  ifdef __cplusplus
}
#  endif

#endif /* LIBFSW_H */