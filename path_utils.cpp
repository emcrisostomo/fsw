#include "path_utils.h"
#include "fsw_log.h"
#include <dirent.h>

using namespace std;

void get_directory_children(const string &path, vector<string> &children)
{
  DIR *dir = ::opendir(path.c_str());

  if (!dir)
  {
    fsw_perror("opendir");
    return;
  }

  while (struct dirent *ent = readdir(dir))
  {
    children.push_back(ent->d_name);
  }

  ::closedir(dir);
}
