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

using namespace std;

typedef struct FSW_SESSION
{
  vector<string> paths;
  monitor *monitor;
  CEVENT_CALLBACK callback;
} FSW_SESSION;

static bool srand_initialized = false;
static fsw_hash_map<FSW_HANDLE, FSW_SESSION> sessions;
static std::mutex session_mutex;
static FSW_THREAD_LOCAL unsigned int last_error;

// Default library callback.
CPP_EVENT_CALLBACK libfsw_cpp_callback_proxy;
FSW_SESSION & get_session(const FSW_HANDLE handle);

void libfsw_cpp_callback_proxy(const std::vector<event> & events, void * handle_ptr)
{
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

FSW_HANDLE fsw_init_session()
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

  FSW_SESSION session;

  session.callback = nullptr;
  session.monitor = nullptr;

  // TODO: configure session
  sessions[handle] = session;

  return handle;
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

void create_default_monitor(FSW_HANDLE handle)
{
  // TODO: remove this when this method is called by a public method
  std::lock_guard<std::mutex> session_lock(session_mutex);
  FSW_SESSION & session = get_session(handle);

  if (session.monitor != nullptr)
    throw FSW_ERR_MONITOR_ALREADY_EXISTS;

  FSW_HANDLE *handle_ptr = new FSW_HANDLE(handle);

  // TODO: check that paths is not empty
#if defined(HAVE_CORESERVICES_CORESERVICES_H)
  session.monitor = new fsevent_monitor(session.paths, libfsw_cpp_callback_proxy, (void *) handle_ptr);
#elif defined(HAVE_SYS_EVENT_H)
  session.monitor = new kqueue_monitor(session.paths, libfsw_cpp_callback_proxy, (void *) handle_ptr);
#elif defined(HAVE_SYS_INOTIFY_H)
  session.monitor = new inotify_monitor(session.paths, libfsw_cpp_callback_proxy, (void *) handle_ptr);
#else
  session.monitor = new poll_monitor(session.paths, libfsw_cpp_callback_proxy, (void *) handle_ptr);
#endif
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
