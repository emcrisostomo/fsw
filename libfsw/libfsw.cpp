#include <iostream>
#include <mutex>
#include <time.h>
#include <stdlib.h>
#include "config.h"
#include "libfsw.h"
#include "libfsw_map.h"
#include "filter.h"
#include "monitor.h"

/* Include the definitions of all the available backends. */
#ifdef HAVE_CORESERVICES_CORESERVICES_H
#  include "fsevent_monitor.h"
#endif
#ifdef HAVE_SYS_EVENT_H
#  include "kqueue_monitor.h"
#endif
#ifdef HAVE_SYS_INOTIFY_H
#  include "inotify_monitor.h"
#endif
#include "poll_monitor.h"

using namespace std;

typedef struct FSW_SESSION
{
  FSW_HANDLE handle;
  vector<string> paths;
  fsw_monitor_type type;
  monitor *monitor;
  CEVENT_CALLBACK callback;
  double latency;
  bool recursive;
  bool follow_symlinks;
  vector<monitor_filter> filters;
} FSW_SESSION;

static bool srand_initialized = false;
static fsw_hash_map<FSW_HANDLE, FSW_SESSION> sessions;
static std::mutex session_mutex;
static FSW_THREAD_LOCAL unsigned int last_error;

// Default library callback.
FSW_EVENT_CALLBACK libfsw_cpp_callback_proxy;
FSW_SESSION & get_session(const FSW_HANDLE handle);

void create_monitor(FSW_HANDLE handle, const fsw_monitor_type type);
monitor * create_default_monitor(const FSW_SESSION & session);
monitor * create_fsevents_monitor(const FSW_SESSION & session);
monitor * create_kqueue_monitor(const FSW_SESSION & session);
monitor * create_poll_monitor(const FSW_SESSION & session);
monitor * create_inotify_monitor(const FSW_SESSION & session);

void libfsw_cpp_callback_proxy(const std::vector<event> & events,
                               void * handle_ptr)
{
  if (!handle_ptr)
    throw int(FSW_ERR_MISSING_CONTEXT);

  const FSW_HANDLE * handle = static_cast<FSW_HANDLE *> (handle_ptr);

  cevent ** cevents = static_cast<cevent **> (::malloc(sizeof (cevent) * events.size()));

  if (cevents == nullptr)
    throw int(FSW_ERR_MEMORY);

  for (int i = 0; i < events.size(); ++i)
  {
    const event evt = events[i];
    cevent *cevt = new cevent();

    // Copy event into C event wrapper.
    const string path = evt.get_path();

    // TODO: best way to allocate and copy a char * from string
    cevt->path = static_cast<char *> (::malloc(sizeof (char *) * (path.length() + 1)));
    if (!cevt->path) throw int(FSW_ERR_MEMORY);

    evt.get_path().copy(cevt->path, path.length() + 1);

    cevt->evt_time = evt.get_time();

    const vector<event_flag> flags = evt.get_flags();
    cevt->flags_num = flags.size();

    if (!cevt->flags_num) cevt->flags = nullptr;
    else
    {
      cevt->flags = static_cast<event_flag *> (::malloc(sizeof (event_flag) * cevt->flags_num));
      if (!cevt->flags) throw int(FSW_ERR_MEMORY);
    }

    for (int e = 0; i < cevt->flags_num; ++e)
    {
      cevt->flags[e] = flags[e];
    }

    cevents[i] = cevt;
  }

  std::lock_guard<std::mutex> session_lock(session_mutex);
  FSW_SESSION & session = get_session(*handle);
  (*session.callback)(cevents, events.size());
}

FSW_HANDLE fsw_init_session(const fsw_monitor_type type)
{
  std::lock_guard<std::mutex> session_lock(session_mutex);

  if (!srand_initialized)
  {
    srand(time(nullptr));
    srand_initialized = true;
  }

  int handle;

  do
  {
    handle = rand();
  } while (sessions.find(handle) != sessions.end());

  FSW_SESSION session{};

  session.handle = handle;
  session.type = type;

  // TODO: configure session
  sessions[handle] = session;

  return handle;
}

void create_monitor(const FSW_HANDLE handle, const fsw_monitor_type type)
{
  FSW_SESSION & session = get_session(handle);

  // Check sufficient data is present to build a monitor.
  if (!session.callback)
    throw int(FSW_ERR_CALLBACK_NOT_SET);

  if (session.monitor)
    throw int(FSW_ERR_MONITOR_ALREADY_EXISTS);

  if (!session.paths.size())
    throw int(FSW_ERR_PATHS_NOT_SET);

  monitor * current_monitor;
  switch (type)
  {
  case fsw_monitor_type::fsevents:
    current_monitor = create_fsevents_monitor(session);
    break;
  case fsw_monitor_type::kqueue:
    current_monitor = create_kqueue_monitor(session);
    break;
  case fsw_monitor_type::inotify:
    current_monitor = create_inotify_monitor(session);
    break;
  case fsw_monitor_type::poll:
    current_monitor = create_poll_monitor(session);
    break;
  case fsw_monitor_type::system_default:
    current_monitor = create_default_monitor(session);
    break;
  default:
    throw int(FSW_ERR_UNKNOWN_MONITOR_TYPE);
  }

  session.monitor = current_monitor;
}

monitor * create_default_monitor(const FSW_SESSION & session)
{
#if defined(HAVE_CORESERVICES_CORESERVICES_H)
  return create_fsevents_monitor(session);
#elif defined(HAVE_SYS_EVENT_H)
  return create_kqueue_monitor(session);
#elif defined(HAVE_SYS_INOTIFY_H)
  return create_inotify_monitor(session);
#else
  return create_poll_monitor(session);
#endif
}

monitor * create_fsevents_monitor(const FSW_SESSION & session)
{
#if defined(HAVE_CORESERVICES_CORESERVICES_H)
  FSW_HANDLE * handle_ptr = new FSW_HANDLE(session.handle);

  return new fsevent_monitor(session.paths,
                             libfsw_cpp_callback_proxy,
                             static_cast<void *> (handle_ptr));
#else
  throw int(FSW_ERR_UNKNOWN_MONITOR);
#endif
}

monitor * create_kqueue_monitor(const FSW_SESSION & session)
{
#if defined(HAVE_SYS_EVENT_H)
  FSW_HANDLE * handle_ptr = new FSW_HANDLE(session.handle);

  return new kqueue_monitor(session.paths,
                            libfsw_cpp_callback_proxy,
                            static_cast<void *> (handle_ptr));
#else
  throw int(FSW_ERR_UNKNOWN_MONITOR);
#endif
}

monitor * create_poll_monitor(const FSW_SESSION & session)
{
  FSW_HANDLE * handle_ptr = new FSW_HANDLE(session.handle);

  return new poll_monitor(session.paths,
                          libfsw_cpp_callback_proxy,
                          static_cast<void *> (handle_ptr));
}

monitor * create_inotify_monitor(const FSW_SESSION & session)
{
#if defined(HAVE_SYS_INOTIFY_H)
  FSW_HANDLE * handle_ptr = new FSW_HANDLE(session.handle);

  return new inotify_monitor(session.paths,
                             libfsw_cpp_callback_proxy,
                             static_cast<void *> (handle_ptr));
#else
  throw int(FSW_ERR_UNKNOWN_MONITOR);
#endif
}

void fsw_add_path(const FSW_HANDLE handle, const char * path)
{
  if (!path)
    throw int(FSW_ERR_INVALID_PATH);

  std::lock_guard<std::mutex> session_lock(session_mutex);
  FSW_SESSION & session = get_session(handle);

  session.paths.push_back(path);
}

void fsw_set_callback(const FSW_HANDLE handle, const CEVENT_CALLBACK callback)
{
  if (!callback)
    throw int(FSW_ERR_INVALID_CALLBACK);

  std::lock_guard<std::mutex> session_lock(session_mutex);
  FSW_SESSION & session = get_session(handle);

  session.callback = callback;
}

void fsw_set_latency(const FSW_HANDLE handle, const double latency)
{
  if (latency < 0)
    throw int(FSW_ERR_INVALID_LATENCY);

  std::lock_guard<std::mutex> session_lock(session_mutex);
  FSW_SESSION & session = get_session(handle);
  session.latency = latency;
}

void fsw_set_recursive(const FSW_HANDLE handle, const bool recursive)
{
  std::lock_guard<std::mutex> session_lock(session_mutex);
  FSW_SESSION & session = get_session(handle);

  session.recursive = recursive;
}

void fsw_set_follow_symlinks(const FSW_HANDLE handle,
                             const bool follow_symlinks)
{
  std::lock_guard<std::mutex> session_lock(session_mutex);
  FSW_SESSION & session = get_session(handle);

  session.follow_symlinks = follow_symlinks;
}

void fsw_add_filter(const FSW_HANDLE handle,
                    const cmonitor_filter filter)
{
  std::lock_guard<std::mutex> session_lock(session_mutex);
  FSW_SESSION & session = get_session(handle);

  session.filters.push_back({filter.text, filter.type, filter.case_sensitive, filter.extended});
}

void fsw_destroy_session(const FSW_HANDLE handle)
{
  std::lock_guard<std::mutex> session_lock(session_mutex);
  FSW_SESSION session = get_session(handle);
  sessions.erase(handle);

  if (session.monitor)
  {
    void * context = session.monitor->get_context();

    if (!context)
    {
      session.monitor->set_context(nullptr);
      delete static_cast<FSW_HANDLE *> (context);
    }

    delete session.monitor;
  }
}

FSW_SESSION & get_session(const FSW_HANDLE handle)
{
  if (sessions.find(handle) == sessions.end())
    throw (unsigned int) FSW_ERR_SESSION_UNKNOWN;

  return sessions[handle];
}

monitor * get_monitor(FSW_HANDLE handle)
{
  std::lock_guard<std::mutex> session_lock(session_mutex);
  return static_cast<monitor *> (get_session(handle).monitor);
}

void fsw_watch_path(FSW_HANDLE handle, char * path)
{
  std::lock_guard<std::mutex> session_lock(session_mutex);
  FSW_SESSION & session = get_session(handle);
  session.paths.push_back(path);
}

unsigned int fsw_last_error()
{
  return last_error;
}

bool fsw_is_verbose()
{
  return false;
}

void fsw_set_verbose(FSW_HANDLE handle, bool verbose)
{
  // TODO
}

void fsw_add_filter(FSW_HANDLE handle, char * regex, filter_type type)
{

}
