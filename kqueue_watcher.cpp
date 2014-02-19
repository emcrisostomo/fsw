#include "kqueue_watcher.h"

#ifdef HAVE_SYS_EVENT_H

#include "fsw_exception.h"
#include "fsw_log.h"
#include <iostream>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <cstdio>
#include <cmath>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

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
  { NOTE_LINK, event_flag::Link });
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

bool kqueue_watcher::is_path_watched(const string & path)
{
  return descriptors_by_file_name.find(path) != descriptors_by_file_name.end();
}

bool kqueue_watcher::add_watch(
    const string & path,
    int & descriptor,
    mode_t & mode)
{
  // check if the path is already watched and if it is, remove it
  if (is_path_watched(path))
  {
    remove_watch(path);
  }

  int fd = ::open(path.c_str(),
#ifdef O_EVTONLY
      O_EVTONLY
#else
      O_RDONLY
#endif
      );

  if (fd == -1)
  {
    string err = string("Cannot open ") + path;
    perror(err.c_str());
    return false;
  }

  struct stat fd_stat;

  if (::stat(path.c_str(), &fd_stat) != 0)
  {
    string err = string("Cannot stat() ") + path;
    perror(err.c_str());
    return false;
  }

  // if the descriptor could be opened, track it
  descriptors_by_file_name[path] = fd;
  file_names_by_descriptor[fd] = path;
  file_modes[fd] = fd_stat.st_mode;

  mode = fd_stat.st_mode;
  descriptor = fd;

  return true;
}

static void get_directory_children(const string &path, vector<string> &children)
{
  DIR *dir = ::opendir(path.c_str());

  if (!dir)
  {
    perror("opendir");
    return;
  }

  while (struct dirent *ent = readdir(dir))
  {
    children.push_back(ent->d_name);
  }

  ::closedir(dir);
}

bool kqueue_watcher::watch_path(const string &path)
{
  mode_t mode;
  int fd;
  if (!add_watch(path, fd, mode))
  {
    return false;
  }

  if (!recursive)
    return true;

  if (!S_ISDIR(mode))
    return true;

  vector<string> dirs_to_process;
  dirs_to_process.push_back(path);

  while (dirs_to_process.size() > 0)
  {
    const string current_dir = dirs_to_process.back();
    vector<string> children;

    get_directory_children(current_dir, children);

    dirs_to_process.pop_back();

    for (string child : children)
    {
      if (child.compare(".") == 0 || child.compare("..") == 0)
        continue;

      const string fqpath = current_dir + "/" + child;

      if (!add_watch(fqpath, fd, mode))
        continue;

      if (S_ISDIR(mode))
        dirs_to_process.push_back(fqpath);
    }
  }

  return true;
}

void kqueue_watcher::remove_watch(int fd)
{
  string name = file_names_by_descriptor[fd];
  file_names_by_descriptor.erase(fd);
  descriptors_by_file_name.erase(name);
  file_modes.erase(fd);
  ::close(fd);
}

void kqueue_watcher::remove_watch(const string &path)
{
  int fd = descriptors_by_file_name[path];
  descriptors_by_file_name.erase(path);
  file_names_by_descriptor.erase(fd);
  file_modes.erase(fd);
  ::close(fd);
}

void kqueue_watcher::rescan_pending()
{
  auto fd_pair = descriptors_to_rescan.begin();

  //for (pair<int, bool> fd_pair : descriptors_to_rescan)
  while (fd_pair != descriptors_to_rescan.end())
  {
    int fd = fd_pair->first;

    // get the path of the descriptor and remove it from the
    // (descriptor, path) map
    string fd_path = file_names_by_descriptor[fd];
    remove_watch(fd);

    // Rescan the hierarchy rooted at fd_path.
    // If the path does not exist any longer, nothing needs to be done since
    // kqueue(2) says:
    //
    // EV_DELETE  Events which are attached to file descriptors are
    //            automatically deleted on the last close of the descriptor.
    //
    // If the descriptor which has vanished is a directory, we don't bother
    // EV_DELETE-ing all its children the event from kqueue for the same
    // reason.
    if (!watch_path(fd_path))
    {
      fsw_log("Notice: ");
      fsw_log(fd_path.c_str());
      fsw_log(" cannot be found. Will retry later.\n");
    }

    descriptors_to_rescan.erase(fd_pair++);
  }
}

void kqueue_watcher::scan_root_paths()
{
  for (string path : paths)
  {
    if (is_path_watched(path))
      continue;

    if (!watch_path(path))
    {
      fsw_log("Notice: ");
      fsw_log(path.c_str());
      fsw_log(" cannot be found. Will retry later.\n");
    }
  }
}

void kqueue_watcher::run()
{
  if (kq != -1)
    throw new fsw_exception("kqueue already running.");

  kq = ::kqueue();

  if (kq == -1)
  {
    perror("::kqueue()");
    throw fsw_exception("kqueue failed.");
  }

  while (true)
  {
    // rescan the pending descriptors
    rescan_pending();

    // scan the root paths to check whether someone is missing
    scan_root_paths();

    vector<struct kevent> changes;
    vector<struct kevent> event_list;

    for (pair<int, string> fd_path : file_names_by_descriptor)
    {
      struct kevent change;

      EV_SET(
          &change,
          fd_path.first,
          EVFILT_VNODE,
          EV_ADD | EV_ENABLE,
          NOTE_DELETE | NOTE_EXTEND | NOTE_RENAME | NOTE_WRITE | NOTE_ATTRIB | NOTE_LINK | NOTE_REVOKE,
          0,
          0);

      changes.push_back(change);
      struct kevent event;
      event_list.push_back(event);
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

      if (e.flags & EV_ERROR)
      {
        perror("Event with EV_ERROR");
        continue;
      }

      if (e.fflags)
      {
        descriptors_to_rescan[e.ident] = true;

        vector<event_flag> evt_flags = decode_flags(e.fflags);

        events.push_back(
        { file_names_by_descriptor[e.ident], curr_time, evt_flags });
      }
    }

    if (events.size() > 0)
    {
      callback(events);
    }
  }
}

#endif  /* HAVE_SYS_EVENT_H */
