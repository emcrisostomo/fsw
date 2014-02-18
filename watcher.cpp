#include "watcher.h"
#include "fsw_exception.h"

watcher::watcher(vector<string> paths_to_watch, EVENT_CALLBACK callback) :
    paths(paths_to_watch), callback(callback)
{
  if (callback == nullptr)
  {
    throw new fsw_exception("Callback cannot be null.");
  }
}

void watcher::set_latency(double latency)
{
  if (latency < 0)
  {
    throw fsw_exception("Latency cannot be negative.");
  }

  this->latency = latency;
}

void watcher::set_recursive(bool recursive)
{
  this->recursive = recursive;
}

watcher::~watcher()
{
}
