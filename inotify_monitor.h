#ifndef FSW_INOTIFY_MONITOR_H
#define FSW_INOTIFY_MONITOR_H

#include "config.h"

#ifdef HAVE_SYS_INOTIFY_H

#include "monitor.h"
#include <string>
#include <vector>

class inotify_monitor : public monitor
{
public:
  inotify_monitor(std::vector<std::string> paths, EVENT_CALLBACK callback);
  virtual ~inotify_monitor();

  void run();

private:
  inotify_monitor(const inotify_monitor& orig);
  inotify_monitor& operator=(const inotify_monitor & that);
  
  int inotify = -1;
};

#endif  /* HAVE_SYS_INOTIFY_H */
#endif  /* FSW_INOTIFY_MONITOR_H */
