#include "kqueue_watcher.h"

#ifdef HAVE_SYS_EVENT_H

#include "fsw.h"
#include "fsw_log.h"
#include <iostream>
#include <sys/event.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

kqueue_watcher::kqueue_watcher(vector<string> paths_to_monitor) :
    paths(paths_to_monitor)
{
}

kqueue_watcher::~kqueue_watcher()
{
  if (kq != -1)
    ::close(kq);
  if (kq != -1)
    ::close(myfile);
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
    vector<struct kevent> events;
    vector<int> open_files;

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
      struct kevent event;

      EV_SET(
          &change,
          file,
          EVFILT_VNODE,
          EV_ADD | EV_ENABLE | EV_CLEAR,
          NOTE_DELETE | NOTE_EXTEND | NOTE_RENAME | NOTE_WRITE | NOTE_ATTRIB | NOTE_LINK | NOTE_REVOKE,
          0,
          0);

      changes.push_back(change);
      events.push_back(event);
      open_files.push_back(file);
    }

    if (changes.size() == 0)
    {
      ::sleep(1);
      continue;
    }

    int event_num = ::kevent(
        kq,
        &changes[0],
        changes.size(),
        &events[0],
        events.size(),
        nullptr);

    if (event_num == -1)
    {
      throw new fsw_exception("Invalid event number.");
    }

    cout << "Modified" << endl;

    for (int file : open_files)
    {
      ::close(file);
    }
  }
}

#endif  /* HAVE_SYS_EVENT_H */
