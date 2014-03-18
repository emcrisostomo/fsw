#include "kqueue_monitor.h"

#ifdef HAVE_SYS_EVENT_H

#include "fsw_exception.h"
#include "fsw_log.h"

using namespace std;

inotify_monitor::inotify_monitor(vector<string> paths_to_monitor,
                               EVENT_CALLBACK callback) :
  monitor(paths_to_monitor, callback)
{
}

inotify_monitor::~inotify_monitor()
{
  if (kq != -1) ::close(kq);
}

#endif  /* HAVE_SYS_EVENT_H */
