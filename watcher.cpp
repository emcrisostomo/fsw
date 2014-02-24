#include "watcher.h"
#include "fsw_exception.h"
#include <cstdlib>

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

void watcher::set_exclude(const vector<string> &exclusions)
{
  for (string exclusion : exclusions)
  {
    regex_t regex;
    if(::regcomp(&regex, exclusion.c_str(), 0))
    {
      string err = "An error occurred during the compilation of " + exclusion;
      throw new fsw_exception(err);
    }

    exclude_regex.push_back(regex);
  }
}

bool watcher::accept_path(const string &path)
{
  for (auto re : exclude_regex)
  {
    if(::regexec(&re, path.c_str(), 0, nullptr, 0) == 0)
    {
      return false;
    }
  }

  return true;
}

bool watcher::accept_path(const char *path)
{
  for (auto re : exclude_regex)
    {
      if(::regexec(&re, path, 0, nullptr, 0) == 0)
      {
        return false;
      }
    }

    return true;
}

watcher::~watcher()
{
  for (auto re : exclude_regex)
  {
    ::regfree(&re);
  }

  exclude_regex.clear();
}
