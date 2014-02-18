#include "kqueue_watcher.h"

#ifdef HAVE_SYS_EVENT_H

#include "fsw_exception.h"
#include "fsw_log.h"
#include <iostream>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <cstdio>
#include <map>
#include <cmath>
#include <unistd.h>
#include <fcntl.h>
#include <ftw.h>

typedef struct KqueueFlagType
{
  uint32_t flag;
  event_flag type;
} KqueueFlagType;

static vector<KqueueFlagType> create_flag_type_vector();
static const vector<KqueueFlagType> event_flag_type = create_flag_type_vector();

vector<KqueueFlagType> create_flag_type_vector()
{
  vector<KqueueFlagType> flags;
  flags.push_back(
  { NOTE_DELETE, event_flag::Removed });
  flags.push_back(
  { NOTE_WRITE, event_flag::Updated });
  flags.push_back(
  { NOTE_EXTEND, event_flag::PlatformSpecific });
  flags.push_back(
  { NOTE_ATTRIB, event_flag::AttributeModified });
  flags.push_back(
  { NOTE_LINK, event_flag::PlatformSpecific });
  flags.push_back(
  { NOTE_RENAME, event_flag::Renamed });
  flags.push_back(
  { NOTE_REVOKE, event_flag::PlatformSpecific });

  return flags;
}

kqueue_watcher::kqueue_watcher(
    vector<string> paths_to_monitor,
    EVENT_CALLBACK callback) :
    watcher(paths_to_monitor, callback)
{
}

kqueue_watcher::~kqueue_watcher()
{
  if (kq != -1)
    ::close(kq);
}

static vector<event_flag> decode_flags(uint32_t flag)
{
  vector<event_flag> evt_flags;

  for (KqueueFlagType type : event_flag_type)
  {
    if (flag & type.flag)
    {
      evt_flags.push_back(type.type);
    }
  }

  return evt_flags;
}

static struct timespec create_timespec_from_latency(double latency)
{
  double seconds;
  double nanoseconds = modf(latency, &seconds);
  nanoseconds *= 1000000000;

  struct timespec ts;
  ts.tv_sec = seconds;
  ts.tv_nsec = nanoseconds;

  return ts;
}

void kqueue_watcher::run()
{
  kq = ::kqueue();

  if (kq == -1)
  {
    throw fsw_exception("kqueue failed.");
  }

  while (true)
  {
    vector<struct kevent> changes;
    vector<struct kevent> event_list;
    vector<int> open_files;
    map<int, string> file_names;

    for (string path : paths)
    {
      int file = ::open(path.c_str(),
#ifdef O_EVTONLY
          O_EVTONLY
#else
          O_RDONLY
#endif
          );
      if (file == -1)
      {
        fsw_log("Notice: ");
        fsw_log(path.c_str());
        fsw_log(" cannot be found. Skipping.\n");

        continue;
      }

      if (recursive)
      {

      }

      struct kevent change;

      EV_SET(
          &change,
          file,
          EVFILT_VNODE,
          EV_ADD | EV_ENABLE | EV_CLEAR,
          NOTE_DELETE | NOTE_EXTEND | NOTE_RENAME | NOTE_WRITE | NOTE_ATTRIB | NOTE_LINK | NOTE_REVOKE,
          0,
          0);

      changes.push_back(change);
      struct kevent event;
      event_list.push_back(event);
      open_files.push_back(file);
      file_names[file] = path;
    }

    if (changes.size() == 0)
    {
      ::sleep(latency > MIN_SPIN_LATENCY ? latency : MIN_SPIN_LATENCY);
      continue;
    }

    struct timespec ts = create_timespec_from_latency(latency);

    int event_num = ::kevent(
        kq,
        &changes[0],
        changes.size(),
        &event_list[0],
        event_list.size(),
        &ts);

    if (event_num == -1)
    {
      perror("::kevent returned -1");
      throw new fsw_exception("Invalid event number.");
    }

    time_t curr_time;
    time(&curr_time);
    vector<event> events;

    for (auto i = 0; i < event_num; ++i)
    {
      struct kevent e = event_list[i];

      if (e.fflags & EV_ERROR)
      {
        perror("Event with EV_ERROR");
        continue;
      }

      if (e.fflags)
      {
        vector<event_flag> evt_flags = decode_flags(e.fflags);

        events.push_back(
        { file_names[e.ident], curr_time, evt_flags });
      }
    }

    if (events.size() > 0)
    {
      callback(events);
    }

    for (int file : open_files)
    {
      ::close(file);
    }
  }
}

#endif  /* HAVE_SYS_EVENT_H */
