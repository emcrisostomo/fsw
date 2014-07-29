#include <iostream>
#include <mutex>
#include <time.h>
#include <stdlib.h>
#include "libfsw.h"
#include "libfsw_map.h"

using namespace std;

typedef struct FSW_SESSION
{
  bool is_verbose;
} FSW_SESSION;

static bool srand_initialized = false;
static fsw_hash_map<FSW_HANDLE, FSW_SESSION> sessions;
static std::mutex session_mutex;
static FSW_THREAD_LOCAL unsigned int last_error;

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
    handle = abs(rand());
  }
  while (sessions.find(handle) != sessions.end());

  FSW_SESSION session;

  // TODO: configure session
  sessions[handle] = session;

  return handle;
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

void fsw_add_filter(FSW_HANDLE handle, char * regex)
{

}
