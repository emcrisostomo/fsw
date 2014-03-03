#ifndef FSW_POLL_MONITOR_H
#define FSW_POLL_MONITOR_H

#include "config.h"
#include "monitor.h"
#include <sys/stat.h>
#include <ctime>
#include "fsw_map.h"

class poll_monitor : public monitor
{
public:
  poll_monitor(std::vector<std::string> paths, EVENT_CALLBACK callback);
  virtual ~poll_monitor();
  void run();

  static const unsigned int MIN_POLL_LATENCY = 1;

private:
  poll_monitor(const poll_monitor& orig);
  poll_monitor& operator=(const poll_monitor & that);

  typedef bool (poll_monitor::*poll_monitor_scan_callback)(
    const std::string &path,
    const struct stat &stat);

  typedef struct watched_file_info
  {
    time_t mtime;
    time_t ctime;
  } watched_file_info;

  typedef struct poll_monitor_data
  {
    fsw_hash_map<std::string, watched_file_info> tracked_files;
  } poll_monitor_data;

  void scan(const std::string &path, poll_monitor_scan_callback fn);
  void collect_initial_data();
  void collect_data();
  bool add_path(const std::string &path,
                const struct stat &fd_stat,
                poll_monitor_scan_callback poll_callback);
  bool initial_scan_callback(const std::string &path, const struct stat &stat);
  bool intermediate_scan_callback(const std::string &path,
                                  const struct stat &stat);
  void find_removed_files();
  void notify_events();
  void swap_data_containers();

  poll_monitor_data *previous_data;
  poll_monitor_data *new_data;

  std::vector<event> events;
  time_t curr_time;
};

#endif  /* FSW_POLL_MONITOR_H */
