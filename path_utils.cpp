#include "path_utils.h"
#include "fsw.h"
#include "fsw_log.h"
#include <dirent.h>
#include <cstdlib>
#include <errno.h>
#include <iostream>

using namespace std;

void get_directory_children(const string &path, vector<string> &children)
{
  DIR *dir = ::opendir(path.c_str());

  if (!dir)
  {
    if (errno == EMFILE || errno == ENFILE)
    {
      perror("opendir");
      ::exit(FSW_EXIT_ENFILE);
    } else {
      fsw_perror("opendir");
    }
    return;
  }

  while (struct dirent * ent = readdir(dir))
  {
    children.push_back(ent->d_name);
  }

  ::closedir(dir);
}

bool read_link_path(const string &path, string &link_path)
{
  char *real_path = ::realpath(path.c_str(), nullptr);
  link_path = (real_path ? real_path : path);

  return (real_path != nullptr);
}

bool stat_path(const string &path, struct stat &fd_stat)
{
  if (::lstat(path.c_str(), &fd_stat) != 0)
  {
    string err = string("Cannot stat() ") + path;
    fsw_perror(err.c_str());
    return false;
  }

  return true;
}
