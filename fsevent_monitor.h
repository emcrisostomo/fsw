#ifndef FSW_FSEVENT_MONITOR_H
#define FSW_FSEVENT_MONITOR_H

#include "config.h"

#ifdef HAVE_CORESERVICES_CORESERVICES_H

#include "monitor.h"
#include <CoreServices/CoreServices.h>

class fsevent_monitor : public monitor
{
public:
  fsevent_monitor(std::vector<std::string> paths, EVENT_CALLBACK callback);
  virtual ~fsevent_monitor();

  void run();
  void set_numeric_event(bool numeric);

private:
  fsevent_monitor(const fsevent_monitor& orig);
  fsevent_monitor& operator=(const fsevent_monitor & that);

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
#endif  /* FSW_FSEVENT_MONITOR_H */
