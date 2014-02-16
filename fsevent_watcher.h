#ifndef FSW_FSEVENT_WATCHER_H
#define FSW_FSEVENT_WATCHER_H

#include "config.h"

#ifdef HAVE_CORESERVICES_CORESERVICES_H

#include "watcher.h"
#include <CoreServices/CoreServices.h>
#include <string>
#include <vector>

using namespace std;

class fsevent_watcher : public watcher
{
public:
  fsevent_watcher(vector<string> paths);
  virtual ~fsevent_watcher();

  void run();
  void set_latency(double latency);
  void set_time_format(const string &format);
  void set_utc_time(bool utc);
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
  double event_latency = 1.0;
  string time_format = "%c";
  bool utc_time = false;
  bool numeric_event = false;

  static const unsigned int TIME_FORMAT_BUFF_SIZE = 128;
};

#endif  /* HAVE_CORESERVICES_CORESERVICES_H */
#endif  /* FSW_FSEVENT_WATCHER_H */
