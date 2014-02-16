#include "event.h"

event::event(string path, time_t evt_time) :
    path(path), evt_time(evt_time)
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
