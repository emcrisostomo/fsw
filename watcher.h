#ifndef FSW__WATCHER_H
#define FSW__WATCHER_H

#include <vector>
#include <string>

using namespace std;

typedef void (*EVENT_CALLBACK)();

class watcher
{
public:
  watcher(vector<string> paths, EVENT_CALLBACK callback);
  virtual ~watcher();

  virtual void run() = 0;

protected:
  vector<string> paths;
};

#endif  /* FSW__WATCHER_H */
