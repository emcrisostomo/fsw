#ifndef FSW_EVENT_H
#define FSW_EVENT_H

#include <string>
#include <ctime>
#include <vector>

using namespace std;

enum class event_flag
{
  PlatformSpecific,
  Created,
  Updated,
  Removed,
  Renamed,
  OwnerModified,
  AttributeModified,
  IsFile,
  IsDir,
  IsSymLink
};

class event
{
public:
  event(string path, time_t evt_time, vector<event_flag> flags);
  virtual ~event();
  string get_path();
  time_t get_time();
  vector<event_flag> get_flags();

private:
  string path;
  time_t evt_time;
  vector<event_flag> evt_flags;
};

#endif  /* FSW_EVENT_H */
