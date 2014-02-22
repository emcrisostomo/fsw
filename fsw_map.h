#ifndef FSW_MAP_H
#define FSW_MAP_H

#include "config.h"

#if defined(HAVE_UNORDERED_MAP)
#include <unordered_map>

template <typename K, typename V>
using fsw_hash_map = unordered_map<K, V>;

#else
#include <map>

template <typename K, typename V>
using fsw_hash_map = map<K, V>;

#endif

#endif  /* FSW_MAP_H */
