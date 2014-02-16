#ifndef FSW_KQUEUE_WATCHER_H
#define FSW_KQUEUE_WATCHER_H

#include <string>
#include <vector>

using namespace std;

class kqueue_watcher
{
public:
  kqueue_watcher(vector<string> paths);
  virtual ~kqueue_watcher();

  void run();

private:
  kqueue_watcher(const kqueue_watcher& orig);
  kqueue_watcher& operator=(const kqueue_watcher & that);

  vector<string> paths;
  int kq = -1;
  int myfile = -1;
};

#endif  /* FSW_KQUEUE_WATCHER_H */
