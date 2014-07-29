#ifndef LIBFSW_H
#  define LIBFSW_H

#  ifdef __cplusplus
extern "C"
{
#  endif

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