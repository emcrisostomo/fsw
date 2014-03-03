#ifndef FSW_PATH_UTILS_H
#define FSW_PATH_UTILS_H

#include <string>
#include <vector>
#include <sys/stat.h>

void get_directory_children(const std::string &path,
                            std::vector<std::string> &children);
bool read_link_path(const std::string &path, std::string &link_path);
bool stat_path(const std::string &path, struct stat &fd_stat);

#endif  /* FSW_PATH_UTILS_H */
