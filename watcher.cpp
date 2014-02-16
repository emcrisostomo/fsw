#include "watcher.h"
#include "fsw_exception.h"

watcher::watcher(vector<string> paths_to_watch, EVENT_CALLBACK callback) :
    paths(paths_to_watch)
{
}

void watcher::set_latency(double latency)
{
  if (latency < 0)
  {
    throw fsw_exception("Latency cannot be negative.");
  }

  this->latency = latency;
}

watcher::~watcher()
{
}
