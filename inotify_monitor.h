#ifndef FSW_INOTIFY_MONITOR_H
#define FSW_INOTIFY_MONITOR_H

#include "config.h"

#ifdef HAVE_SYS_INOTIFY_H

#include "monitor.h"
#include <sys/inotify.h>
#include <ctime>
#include <string>
#include <vector>
#include "fsw_map.h"

class inotify_monitor : public monitor
{
public:
  inotify_monitor(std::vector<std::string> paths, EVENT_CALLBACK callback);
  virtual ~inotify_monitor();

  void run();

private:
  inotify_monitor(const inotify_monitor& orig);
  inotify_monitor& operator=(const inotify_monitor & that);
  
  void collect_initial_data();
  void notify_events();
  void preprocess_dir_event(struct inotify_event * event);
  void preprocess_event(struct inotify_event * event);
  void preprocess_node_event(struct inotify_event * event);
  void scan(const std::string &path);
  
  int inotify = -1;
  std::vector<event> events;
  fsw_hash_map<int, std::string> file_names_by_descriptor;
  time_t curr_time;
};

#endif  /* HAVE_SYS_INOTIFY_H */
#endif  /* FSW_INOTIFY_MONITOR_H */
