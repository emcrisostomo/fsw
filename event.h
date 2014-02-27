#ifndef FSW_EVENT_H
#define FSW_EVENT_H

#include <string>
#include <ctime>
#include <vector>

using namespace std;

enum class event_flag
{
  PlatformSpecific = 1,
  Created = 2,
  Updated = 4,
  Removed = 8,
  Renamed = 16,
  OwnerModified = 32,
  AttributeModified = 64,
  IsFile = 128,
  IsDir = 256,
  IsSymLink = 512,
  Link = 1024
};

class event
{
public:
  event(string path, time_t evt_time, vector<event_flag> flags);
  virtual ~event();
  string get_path() const;
  time_t get_time() const;
  vector<event_flag> get_flags() const;

private:
  string path;
  time_t evt_time;
  vector<event_flag> evt_flags;
};

#endif  /* FSW_EVENT_H */
