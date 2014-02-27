#ifndef FSW__WATCHER_H
#define FSW__WATCHER_H

#include "config.h"
#include <vector>
#include <string>
#ifdef HAVE_REGCOMP
#include <regex.h>
#endif
#include "event.h"

using namespace std;

typedef void (*EVENT_CALLBACK)(const vector<event> &);

class watcher
{
public:
  watcher(vector<string> paths, EVENT_CALLBACK callback);
  virtual ~watcher();
  void set_latency(double latency);
  void set_recursive(bool recursive);
  void set_exclude(
      const vector<string> &exclusions,
      bool case_sensitive = true,
      bool extended = false);

  virtual void run() = 0;

protected:
  bool accept_path(const string &path);
  bool accept_path(const char *path);

protected:
  vector<string> paths;
  EVENT_CALLBACK callback;
  double latency = 1.0;
  bool recursive = false;

private:
#ifdef HAVE_REGCOMP
  vector<regex_t> exclude_regex;
#endif
};

#endif  /* FSW__WATCHER_H */
