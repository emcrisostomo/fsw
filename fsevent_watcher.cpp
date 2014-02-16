#include "fsevent_watcher.h"

#ifdef HAVE_CORESERVICES_CORESERVICES_H

#include "fsw.h"
#include "fsw_log.h"
#include <ctime>
#include <iostream>

typedef struct FSEventFlagName
{
  FSEventStreamEventFlags flag;
  string name;
} FSEventFlagName;

static vector<FSEventFlagName> event_names =
{
{ kFSEventStreamEventFlagNone, "none" },
{ kFSEventStreamEventFlagMustScanSubDirs, "mustScanSubdir" },
{ kFSEventStreamEventFlagUserDropped, "userDropped" },
{ kFSEventStreamEventFlagKernelDropped, "kernelDropped" },
{ kFSEventStreamEventFlagEventIdsWrapped, "idsWrapped" },
{ kFSEventStreamEventFlagHistoryDone, "historyDone" },
{ kFSEventStreamEventFlagRootChanged, "rootChanged" },
{ kFSEventStreamEventFlagMount, "mount" },
{ kFSEventStreamEventFlagUnmount, "unmount" },
{ kFSEventStreamEventFlagItemCreated, "created" },
{ kFSEventStreamEventFlagItemRemoved, "removed" },
{ kFSEventStreamEventFlagItemInodeMetaMod, "inodeMetaMod" },
{ kFSEventStreamEventFlagItemRenamed, "renamed" },
{ kFSEventStreamEventFlagItemModified, "modified" },
{ kFSEventStreamEventFlagItemFinderInfoMod, "finderInfoMod" },
{ kFSEventStreamEventFlagItemChangeOwner, "changeOwner" },
{ kFSEventStreamEventFlagItemXattrMod, "xattrMod" },
{ kFSEventStreamEventFlagItemIsFile, "isFile" },
{ kFSEventStreamEventFlagItemIsDir, "isDir" },
{ kFSEventStreamEventFlagItemIsSymlink, "isSymLink" } };

fsevent_watcher::fsevent_watcher(vector<string> paths_to_monitor, EVENT_CALLBACK callback) :
    watcher(paths_to_monitor, callback)
{
}

fsevent_watcher::~fsevent_watcher()
{
  if (stream)
  {
    fsw_log("Stopping event stream...\n");
    FSEventStreamStop(stream);

    fsw_log("Invalidating event stream...\n");
    FSEventStreamInvalidate(stream);

    fsw_log("Releasing event stream...\n");
    FSEventStreamRelease(stream);
  }

  stream = nullptr;
}

void fsevent_watcher::set_latency(double latency)
{
  event_latency = latency;
}

void fsevent_watcher::set_time_format(const string &format)
{
  time_format = string(format);
}

void fsevent_watcher::set_utc_time(bool utc)
{
  utc_time = utc;
}

void fsevent_watcher::set_numeric_event(bool numeric)
{
  numeric_event = numeric;
}

void fsevent_watcher::run()
{
  if (stream)
    return;

  // parsing paths
  vector<CFStringRef> dirs;

  for (string path : paths)
  {
    fsw_log("Adding path: ");
    fsw_log(path.c_str());
    fsw_log("\n");

    dirs.push_back(
        CFStringCreateWithCString(NULL, path.c_str(), kCFStringEncodingUTF8));
  }

  CFArrayRef pathsToWatch = CFArrayCreate(
      NULL,
      reinterpret_cast<const void **>(&dirs[0]),
      dirs.size(),
      &kCFTypeArrayCallBacks);

  FSEventStreamContext *context = new FSEventStreamContext();
  context->version = 0;
  context->info = this;
  context->retain = nullptr;
  context->release = nullptr;
  context->copyDescription = nullptr;

  CFAbsoluteTime latency = event_latency;

  fsw_log("Creating FSEvent stream...\n");
  stream = FSEventStreamCreate(
      NULL,
      &fsevent_watcher::fsevent_callback,
      context,
      pathsToWatch,
      kFSEventStreamEventIdSinceNow,
      latency,
      kFSEventStreamCreateFlagFileEvents);

  if (!stream)
  {
    throw fsw_exception("Event stream could not be created.");
  }

  fsw_log("Scheduling stream with run loop...\n");
  FSEventStreamScheduleWithRunLoop(
      stream,
      CFRunLoopGetCurrent(),
      kCFRunLoopDefaultMode);

  fsw_log("Starting event stream...\n");
  FSEventStreamStart(stream);

  fsw_log("Starting run loop...\n");
  CFRunLoopRun();
}

void print_numeric_flag(FSEventStreamEventFlags flag)
{
  cout << flag;
}

void print_text_flag(FSEventStreamEventFlags flag)
{
  for (FSEventFlagName name : event_names)
  {
    if (flag & name.flag)
    {
      cout << name.name << " ";
    }
  }
}

void fsevent_watcher::fsevent_callback(
    ConstFSEventStreamRef streamRef,
    void *clientCallBackInfo,
    size_t numEvents,
    void *eventPaths,
    const FSEventStreamEventFlags eventFlags[],
    const FSEventStreamEventId eventIds[])
{
  fsevent_watcher *watcher =
      reinterpret_cast<fsevent_watcher *>(clientCallBackInfo);

  if (!watcher)
  {
    throw fsw_exception("The callback info cannot be cast to fsevent_watcher.");
  }

  time_t curr_time;
  time(&curr_time);
  char time_format_buffer[fsevent_watcher::TIME_FORMAT_BUFF_SIZE];
  struct tm * tm_time =
      watcher->utc_time ? gmtime(&curr_time) : localtime(&curr_time);

  string date =
      strftime(
          time_format_buffer,
          TIME_FORMAT_BUFF_SIZE,
          watcher->time_format.c_str(),
          tm_time) ? string(time_format_buffer) : string("<date format error>");

  for (size_t i = 0; i < numEvents; ++i)
  {
    cout << date << " - " << ((char **) eventPaths)[i] << ":";

    if (watcher->numeric_event)
    {
      print_numeric_flag(eventFlags[i]);
    }
    else
    {
      print_text_flag(eventFlags[i]);
    }

    cout << endl;
  }
}

#endif  /* HAVE_CORESERVICES_CORESERVICES_H */
