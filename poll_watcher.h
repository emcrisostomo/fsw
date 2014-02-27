#ifndef FSW_POLL_WATCHER_H
#define FSW_POLL_WATCHER_H

#include "config.h"
#include "watcher.h"
#include <sys/stat.h>
#include <ctime>
#include "fsw_map.h"

class poll_watcher: public watcher
{
public:
  poll_watcher(std::vector<std::string> paths, EVENT_CALLBACK callback);
  virtual ~poll_watcher();

  void run();

  static const unsigned int MIN_POLL_LATENCY = 1;

private:
  poll_watcher(const poll_watcher& orig);
  poll_watcher& operator=(const poll_watcher & that);

  typedef void (poll_watcher::*poll_watcher_scan_callback)(
      const std::string &path,
      struct stat &stat);

  typedef struct watched_file_info {
    time_t mtime;
    time_t ctime;
  } watched_file_info;

  typedef struct poll_watcher_data
  {
    fsw_hash_map<std::string, watched_file_info> tracked_files;
  } poll_watcher_data;

  void scan(const std::string &path, poll_watcher_scan_callback fn);
  void collect_initial_data();
  void collect_data();
  bool add_path(
      const std::string &path,
      mode_t & mode,
      poll_watcher_scan_callback poll_callback);
  void initial_scan_callback(const std::string &path, struct stat &stat);
  void intermediate_scan_callback(const std::string &path, struct stat &stat);
  void find_removed_files();
  void notify_events();
  void swap_data_containers();

  poll_watcher_data *previous_data;
  poll_watcher_data *new_data;

  std::vector<event> events;
  time_t curr_time;
};

#endif  /* FSW_POLL_WATCHER_H */
