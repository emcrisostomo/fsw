#ifndef FSW_KQUEUE_WATCHER_H
#define FSW_KQUEUE_WATCHER_H

#include "config.h"

#ifdef HAVE_SYS_EVENT_H

#include "watcher.h"
#include <string>
#include <vector>

using namespace std;

class kqueue_watcher : public watcher
{
public:
  kqueue_watcher(vector<string> paths, EVENT_CALLBACK callback);
  virtual ~kqueue_watcher();

  void run();

private:
  kqueue_watcher(const kqueue_watcher& orig);
  kqueue_watcher& operator=(const kqueue_watcher & that);

  int kq = -1;
};

#endif  /* HAVE_SYS_EVENT_H */
#endif  /* FSW_KQUEUE_WATCHER_H */
