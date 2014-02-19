#include "fsw_log.h"
#include "fsw.h"
#include <iostream>

using namespace std;

void fsw_log(const char * msg)
{
  if (is_verbose())
  {
    cout << msg;
  }
}

void fsw_perror(const char * msg)
{
  if (is_verbose())
  {
    perror(msg);
  }
}
