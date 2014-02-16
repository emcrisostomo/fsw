#ifndef FSW__WATCHER_H
#define FSW__WATCHER_H

#include <vector>
#include <string>

using namespace std;

class watcher
{
public:
  watcher(vector<string> paths);
  virtual ~watcher();

  virtual void run() = 0;

protected:
  vector<string> paths;
};

#endif  /* FSW__WATCHER_H */
