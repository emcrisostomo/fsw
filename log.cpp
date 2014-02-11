#include "log.h"
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
