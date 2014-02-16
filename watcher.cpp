#include "watcher.h"

watcher::watcher(vector<string> paths_to_watch, EVENT_CALLBACK callback) :
    paths(paths_to_watch)
{
}

watcher::~watcher()
{
}
