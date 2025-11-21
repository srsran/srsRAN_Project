/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/flat_map.h"
#include "srsran/adt/static_vector.h"

namespace srsran {

/// Flat map implementation using static_vector as underlying storage.
template <typename Key, typename Value, std::size_t Capacity, typename Comp = std::less<Key>>
using static_flat_map = flat_map<Key, Value, Comp, static_vector<Key, Capacity>, static_vector<Value, Capacity>>;

} // namespace srsran
