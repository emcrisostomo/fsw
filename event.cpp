#include "event.h"

event::event(string path, time_t evt_time, vector<event_flag> flags) :
    path(path), evt_time(evt_time), evt_flags(flags)
{
}

event::~event()
{
}

string event::get_path()
{
  return path;
}

time_t event::get_time()
{
  return evt_time;
}

vector<event_flag> event::get_flags()
{
  return evt_flags;
}
