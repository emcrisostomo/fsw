#ifndef FSW__WATCHER_H
#define FSW__WATCHER_H

#include <vector>
#include <string>
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

  virtual void run() = 0;

protected:
  vector<string> paths;
  EVENT_CALLBACK callback;
  double latency = 1.0;
  bool recursive = false;
};

#endif  /* FSW__WATCHER_H */
