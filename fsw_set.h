#ifndef FSW_SET_H
#define FSW_SET_H

#include "config.h"

#if defined(HAVE_UNORDERED_SET)
#include <unordered_set>

template <typename K>
using fsw_hash_set = unordered_set<K>;

#else
#include <map>

template <typename K>
using fsw_hash_set = set<K>;

#endif

#endif  /* FSW_SET_H */
