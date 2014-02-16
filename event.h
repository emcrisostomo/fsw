#ifndef FSW_EVENT_H
#define FSW_EVENT_H

#include <string>
#include <ctime>

using namespace std;

class event
{
public:
  event(string path, time_t evt_time);
  virtual ~event();
  string get_path();
  time_t get_time();

private:
  string path;
  time_t evt_time;
};

#endif  /* FSW_EVENT_H */
