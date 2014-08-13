/* 
 * Copyright (C) 2014, Enrico M. Crisostomo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifdef HAVE_CONFIG_H
#  include "libfsw_config.h"
#endif

#include <iostream>
#include <mutex>
#include <time.h>
#include <stdlib.h>

#include "libfsw.h"

#ifdef HAVE_CXX_THREAD
#  include <thread>
#endif
#include "../c++/libfsw_map.h"
#include "../c++/filter.h"
#include "../c++/monitor.h"

/* Include the definitions of all the available backends. */
#ifdef HAVE_CORESERVICES_CORESERVICES_H
#  include "../c++/fsevent_monitor.h"
#endif
#ifdef HAVE_SYS_EVENT_H
#  include "../c++/kqueue_monitor.h"
#endif
#ifdef HAVE_SYS_INOTIFY_H
#  include "../c++/inotify_monitor.h"
#endif
#include "../c++/poll_monitor.h"
#include "../c++/libfsw_exception.h"

using namespace std;
using namespace fsw;

typedef struct FSW_SESSION
{
  FSW_HANDLE handle;
  vector<string> paths;
  fsw_monitor_type type;
  monitor *monitor;
  FSW_CEVENT_CALLBACK callback;
  double latency;
  bool recursive;
  bool follow_symlinks;
  vector<monitor_filter> filters;
  bool running;
} FSW_SESSION;

static bool srand_initialized = false;
static fsw_hash_map<FSW_HANDLE, FSW_SESSION> sessions;
#ifdef HAVE_CXX_THREAD
static fsw_hash_map<FSW_HANDLE, thread> monitor_threads;
#endif
static std::mutex session_mutex;
static std::mutex thread_mutex;
#if defined(HAVE_CXX_THREAD_LOCAL)
static FSW_THREAD_LOCAL unsigned int last_error;
#endif

// Default library callback.
FSW_EVENT_CALLBACK libfsw_cpp_callback_proxy;
FSW_SESSION & get_session(const FSW_HANDLE handle);

int create_monitor(FSW_HANDLE handle, const fsw_monitor_type type);

void libfsw_cpp_callback_proxy(const std::vector<event> & events,
                               void * handle_ptr)
{
  // TODO: A C friendly error handler should be notified instead of throwing an exception.
  if (!handle_ptr)
    throw int(FSW_ERR_MISSING_CONTEXT);

  const FSW_HANDLE * handle = static_cast<FSW_HANDLE *> (handle_ptr);

  fsw_cevent ** cevents = static_cast<fsw_cevent **> (::malloc(sizeof (fsw_cevent) * events.size()));

  if (cevents == nullptr)
    throw int(FSW_ERR_MEMORY);

  for (int i = 0; i < events.size(); ++i)
  {
    const event evt = events[i];
    fsw_cevent *cevt = new fsw_cevent();

    // Copy event into C event wrapper.
    const string path = evt.get_path();

    // TODO: best way to allocate and copy a char * from string
    cevt->path = static_cast<char *> (::malloc(sizeof (char *) * (path.length() + 1)));
    if (!cevt->path) throw int(FSW_ERR_MEMORY);

    evt.get_path().copy(cevt->path, path.length() + 1);

    cevt->evt_time = evt.get_time();

    const vector<fsw_event_flag> flags = evt.get_flags();
    cevt->flags_num = flags.size();

    if (!cevt->flags_num) cevt->flags = nullptr;
    else
    {
      cevt->flags = static_cast<fsw_event_flag *> (::malloc(sizeof (fsw_event_flag) * cevt->flags_num));
      if (!cevt->flags) throw int(FSW_ERR_MEMORY);
    }

    for (int e = 0; e < cevt->flags_num; ++e)
    {
      cevt->flags[e] = flags[e];
    }

    cevents[i] = cevt;
  }

  // TODO manage C++ exceptions from C code
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

  sessions[handle] = session;

  return handle;
}

int create_monitor(const FSW_HANDLE handle, const fsw_monitor_type type)
{
  try
  {
    FSW_SESSION & session = get_session(handle);

    // Check sufficient data is present to build a monitor.
    if (!session.callback)
      return fsw_set_last_error(int(FSW_ERR_CALLBACK_NOT_SET));

    if (session.monitor)
      return fsw_set_last_error(int(FSW_ERR_MONITOR_ALREADY_EXISTS));

    if (!session.paths.size())
      return fsw_set_last_error(int(FSW_ERR_PATHS_NOT_SET));

    FSW_HANDLE * handle_ptr = new FSW_HANDLE(session.handle);
    monitor * current_monitor = monitor::create_monitor(type, 
                                                        session.paths, 
                                                        libfsw_cpp_callback_proxy, 
                                                        handle_ptr);
    session.monitor = current_monitor;
  }
  catch (libfsw_exception ex)
  {
    return fsw_set_last_error(int(ex));
  }
  catch (int error)
  {
    return fsw_set_last_error(error);
  }

  return fsw_set_last_error(FSW_OK);
}

int fsw_add_path(const FSW_HANDLE handle, const char * path)
{
  if (!path)
    return fsw_set_last_error(int(FSW_ERR_INVALID_PATH));

  try
  {
    std::lock_guard<std::mutex> session_lock(session_mutex);
    FSW_SESSION & session = get_session(handle);

    session.paths.push_back(path);
  }
  catch (int error)
  {
    return fsw_set_last_error(error);
  }

  return fsw_set_last_error(FSW_OK);
}

int fsw_set_callback(const FSW_HANDLE handle, const FSW_CEVENT_CALLBACK callback)
{
  if (!callback)
    return fsw_set_last_error(int(FSW_ERR_INVALID_CALLBACK));

  try
  {
    std::lock_guard<std::mutex> session_lock(session_mutex);
    FSW_SESSION & session = get_session(handle);

    session.callback = callback;
  }
  catch (int error)
  {
    return fsw_set_last_error(error);
  }

  return fsw_set_last_error(FSW_OK);
}

int fsw_set_latency(const FSW_HANDLE handle, const double latency)
{
  if (latency < 0)
    return fsw_set_last_error(int(FSW_ERR_INVALID_LATENCY));

  try
  {
    std::lock_guard<std::mutex> session_lock(session_mutex);
    FSW_SESSION & session = get_session(handle);

    session.latency = latency;
  }
  catch (int error)
  {
    return fsw_set_last_error(error);
  }

  return fsw_set_last_error(FSW_OK);
}

int fsw_set_recursive(const FSW_HANDLE handle, const bool recursive)
{
  try
  {
    std::lock_guard<std::mutex> session_lock(session_mutex);
    FSW_SESSION & session = get_session(handle);

    session.recursive = recursive;
  }
  catch (int error)
  {
    return fsw_set_last_error(error);
  }

  return fsw_set_last_error(FSW_OK);
}

int fsw_set_follow_symlinks(const FSW_HANDLE handle,
                            const bool follow_symlinks)
{
  try
  {
    std::lock_guard<std::mutex> session_lock(session_mutex);
    FSW_SESSION & session = get_session(handle);

    session.follow_symlinks = follow_symlinks;
  }
  catch (int error)
  {
    return fsw_set_last_error(error);
  }

  return fsw_set_last_error(FSW_OK);
}

int fsw_add_filter(const FSW_HANDLE handle,
                   const fsw_cmonitor_filter filter)
{
  try
  {
    std::lock_guard<std::mutex> session_lock(session_mutex);
    FSW_SESSION & session = get_session(handle);

    session.filters.push_back({filter.text, filter.type, filter.case_sensitive, filter.extended});
  }
  catch (int error)
  {
    return fsw_set_last_error(error);
  }

  return fsw_set_last_error(FSW_OK);
}

int fsw_run_monitor(const FSW_HANDLE handle)
{
  try
  {
    std::lock_guard<std::mutex> session_lock(session_mutex);
    FSW_SESSION & session = get_session(handle);

    if (session.running)
      return fsw_set_last_error(int(FSW_ERR_MONITOR_ALREADY_RUNNING));

#ifdef HAVE_CXX_THREAD
    if (monitor_threads.find(handle) != monitor_threads.end())
      return fsw_set_last_error(int(FSW_ERR_STALE_MONITOR_THREAD));
#endif

    if (!session.monitor)
      create_monitor(handle, session.type);

    session.monitor->set_filters(session.filters);
    session.monitor->set_follow_symlinks(session.follow_symlinks);
    session.monitor->set_latency(session.latency);
    session.monitor->set_recursive(session.recursive);
    session.running = true;

    session.monitor->start();
  }
  catch (system_error & se)
  {
    return fsw_set_last_error(int(FSW_ERR_THREAD_FAULT));
  }
  catch (int error)
  {
    return fsw_set_last_error(error);
  }

  return fsw_set_last_error(FSW_OK);
}

int fsw_monitor_join(const FSW_HANDLE handle)
{
#ifdef HAVE_CXX_THREAD
  try
  {
    std::lock_guard<std::mutex> session_lock(thread_mutex);

    if (monitor_threads.find(handle) == monitor_threads.end())
      return fsw_set_last_error(int(FSW_ERR_UNKNOWN_MONITOR));

    monitor_threads[handle].join();
  }
  catch (system_error & se)
  {
    return fsw_set_last_error(int(FSW_ERR_THREAD_FAULT));
  }
  catch (int error)
  {
    return fsw_set_last_error(error);
  }

  return fsw_set_last_error(FSW_OK);
#else
  return fsw_set_last_error(FSW_ERR_UNSUPPORTED_OPERATION);
#endif
}

int fsw_destroy_session(const FSW_HANDLE handle)
{
  try
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
  catch (int error)
  {
    return fsw_set_last_error(error);
  }

  return fsw_set_last_error(FSW_OK);
}

FSW_SESSION & get_session(const FSW_HANDLE handle)
{
  if (sessions.find(handle) == sessions.end())
    throw int(FSW_ERR_SESSION_UNKNOWN);

  return sessions[handle];
}

int fsw_set_last_error(const int error)
{
#if defined(HAVE_CXX_THREAD_LOCAL)
  last_error = error;
#endif

  return error;
}

int fsw_last_error()
{
#if defined(HAVE_CXX_THREAD_LOCAL)
  return last_error;
#else
  return fsw_set_last_error(FSW_ERR_UNSUPPORTED_OPERATION);
#endif
}

bool fsw_is_verbose()
{
  return false;
}