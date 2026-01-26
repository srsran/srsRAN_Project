/*
 *
 * Copyright 2021-2026 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
