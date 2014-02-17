#include "kqueue_watcher.h"

#ifdef HAVE_SYS_EVENT_H

#include "fsw_exception.h"
#include "fsw_log.h"
#include <iostream>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <map>

typedef struct KqueueFlagType
{
  uint32_t flag;
  event_flag type;
} FSEventFlagType;

static vector<KqueueFlagType> event_flag_type =
{
{ NOTE_DELETE, event_flag::Removed },
{ NOTE_WRITE, event_flag::Updated },
{ NOTE_EXTEND, event_flag::PlatformSpecific },
{ NOTE_ATTRIB, event_flag::AttributeModified },
{ NOTE_LINK, event_flag::PlatformSpecific },
{ NOTE_RENAME, event_flag::Renamed },
{ NOTE_REVOKE, event_flag::PlatformSpecific } };

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
  if (kq != -1)
    ::close(myfile);
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
      int file = ::open(path.c_str(), O_RDONLY);
      if (file == -1)
      {
        fsw_log("Notice: ");
        fsw_log(path.c_str());
        fsw_log(" cannot be found. Skipping.\n");

        continue;
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
      file_names.insert(pair<int, string>(file, path));
    }

    if (changes.size() == 0)
    {
      ::sleep(latency > 1 ? latency : 1);
      continue;
    }

    int event_num = ::kevent(
        kq,
        &changes[0],
        changes.size(),
        &event_list[0],
        event_list.size(),
        nullptr);

    if (event_num == -1)
    {
      throw new fsw_exception("Invalid event number.");
    }

    time_t curr_time;
    time(&curr_time);
    vector<event> events;

    for (auto i = 0; i < event_list.size(); ++i)
    {
      struct kevent e = event_list[i];

      if (e.fflags)
      {
        vector<event_flag> evt_flags = decode_flags(e.fflags);
        event evt =
        { file_names[e.ident], curr_time, evt_flags };

        events.push_back(evt);
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
