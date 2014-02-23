#include "poll_watcher.h"
#include "fsw_log.h"
#include "path_utils.h"
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

poll_watcher::poll_watcher(vector<string> paths, EVENT_CALLBACK callback) :
    watcher(paths, callback)
{
  previous_data = new poll_watcher_data();
  new_data = new poll_watcher_data();
  time(&curr_time);
}

poll_watcher::~poll_watcher()
{
  delete previous_data;
  delete new_data;
}

void poll_watcher::initial_scan_callback(const string &path, struct stat &stat)
{
#if defined(HAVE_CXX_HASH)
  size_t path_hash = str_hash(path);
#else
  string path_hash = path;
#endif
  previous_data->tracked_files[path] = path_hash;
  previous_data->mtime[path_hash] = stat.st_mtimespec;
  previous_data->ctime[path_hash] = stat.st_ctimespec;
}

void poll_watcher::intermediate_scan_callback(
    const string &path,
    struct stat &stat)
{

#if defined(HAVE_CXX_HASH)
  size_t path_hash = str_hash(path);
#else
  string path_hash = path;
#endif

  new_data->tracked_files[path] = path_hash;
  new_data->mtime[path_hash] = stat.st_mtimespec;
  new_data->ctime[path_hash] = stat.st_ctimespec;

  if (previous_data->tracked_files.count(path))
  {
    vector<event_flag> flags;

    if (stat.st_mtimespec.tv_sec > previous_data->mtime[path_hash].tv_sec)
    {
      flags.push_back(event_flag::Updated);
    }

    if (stat.st_ctimespec.tv_sec > previous_data->ctime[path_hash].tv_sec)
    {
      flags.push_back(event_flag::AttributeModified);
    }

    if (flags.size() > 0)
    {
      events.push_back(
      { path, curr_time, flags });
    }

    previous_data->tracked_files.erase(path);
    previous_data->mtime.erase(path_hash);
    previous_data->ctime.erase(path_hash);
  }
  else
  {
    vector<event_flag> flags;
    flags.push_back(event_flag::Created);

    events.push_back(
    { path, curr_time, flags });
  }
}

bool poll_watcher::add_path(
    const string &path,
    mode_t &mode,
    poll_watcher_scan_callback poll_callback)
{
  int o_flags = 0;
#ifdef O_SYMLINK
  o_flags |= O_SYMLINK;
#elif defined(O_NOFOLLOW)
  o_flags |= O_NOFOLLOW;
#endif
#ifdef O_EVTONLY
  o_flags |= O_EVTONLY;
#else
  o_flags |= O_RDONLY;
#endif

  int fd = ::open(path.c_str(), o_flags);

  if (fd == -1)
  {
    string err = string("Cannot open ") + path;
    fsw_perror(err.c_str());

    return false;
  }

  struct stat fd_stat;
  bool has_stat = true;

  if (::stat(path.c_str(), &fd_stat) != 0)
  {
    string err = string("Cannot stat() ") + path;
    fsw_perror(err.c_str());
    has_stat = false;
  }

  ::close(fd);

  if (has_stat)
  {
    mode = fd_stat.st_mode;
    ((*this).*(poll_callback))(path, fd_stat);
  }

  return has_stat;
}

void poll_watcher::scan(const string &path, poll_watcher_scan_callback fn)
{
  mode_t mode;

  if (!add_path(path, mode, fn))
    return;

  if (!recursive)
    return;

  if (!S_ISDIR(mode))
    return;

  vector<string> dirs_to_process;
  dirs_to_process.push_back(path);

  while (dirs_to_process.size() > 0)
  {
    const string current_dir = dirs_to_process.back();
    dirs_to_process.pop_back();

    vector<string> children;
    get_directory_children(current_dir, children);

    for (string child : children)
    {
      if (child.compare(".") == 0 || child.compare("..") == 0)
        continue;

      const string fqpath = current_dir + "/" + child;

      if (!add_path(fqpath, mode, fn))
        continue;

      if (S_ISDIR(mode))
        dirs_to_process.push_back(fqpath);
    }
  }
}

void poll_watcher::find_removed_files()
{
  vector<event_flag> flags;
  flags.push_back(event_flag::Removed);

  for (auto removed : previous_data->tracked_files)
  {
    events.push_back(
    { removed.first, curr_time, flags });
  }
}

void poll_watcher::swap_data_containers()
{
  delete previous_data;
  previous_data = new_data;
  new_data = new poll_watcher_data();
}

void poll_watcher::collect_data()
{
  poll_watcher_scan_callback fn = &poll_watcher::intermediate_scan_callback;

  for (string path : paths)
  {
    scan(path, fn);
  }

  find_removed_files();
  swap_data_containers();
}

void poll_watcher::collect_initial_data()
{
  poll_watcher_scan_callback fn = &poll_watcher::initial_scan_callback;

  for (string path : paths)
  {
    scan(path, fn);
  }
}

void poll_watcher::notify_events()
{
  if (events.size())
  {
    callback(events);
    events.clear();
  }
}

void poll_watcher::run()
{
  collect_initial_data();

  while (true)
  {
    ::sleep(latency < MIN_POLL_LATENCY ? MIN_POLL_LATENCY : latency);

    time(&curr_time);

    collect_data();
    notify_events();
  }
}
