#include "event.h"

using namespace std;

event::event(string path, time_t evt_time, vector<event_flag> flags) :
  path(path), evt_time(evt_time), evt_flags(flags)
{
}

event::~event()
{
}

string event::get_path() const
{
  return path;
}

time_t event::get_time() const
{
  return evt_time;
}

vector<event_flag> event::get_flags() const
{
  return evt_flags;
}
