#ifndef FSW_FSEVENT_WATCHER_H
#define FSW_FSEVENT_WATCHER_H

#include "config.h"

#ifdef HAVE_CORESERVICES_CORESERVICES_H

#include "monitor.h"
#include <CoreServices/CoreServices.h>

class fsevent_watcher : public monitor
{
public:
  fsevent_watcher(std::vector<std::string> paths, EVENT_CALLBACK callback);
  virtual ~fsevent_watcher();

  void run();
  void set_numeric_event(bool numeric);

private:
  fsevent_watcher(const fsevent_watcher& orig);
  fsevent_watcher& operator=(const fsevent_watcher & that);

  static void fsevent_callback(
      ConstFSEventStreamRef streamRef,
      void *clientCallBackInfo,
      size_t numEvents,
      void *eventPaths,
      const FSEventStreamEventFlags eventFlags[],
      const FSEventStreamEventId eventIds[]);

  FSEventStreamRef stream = nullptr;
  bool numeric_event = false;
};

#endif  /* HAVE_CORESERVICES_CORESERVICES_H */
#endif  /* FSW_FSEVENT_WATCHER_H */
