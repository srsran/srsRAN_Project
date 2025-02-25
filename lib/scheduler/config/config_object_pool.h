/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "config_ptr.h"
#include <deque>

namespace srsran {

/// Pool of configuration objects that ensures uniqueness of the stored objects.
template <typename T>
class config_object_pool
{
public:
  config_object_pool()                                     = default;
  config_object_pool(config_object_pool&&)                 = default;
  config_object_pool(const config_object_pool&)            = delete;
  config_object_pool& operator=(config_object_pool&&)      = delete;
  config_object_pool& operator=(const config_object_pool&) = delete;

  /// \brief If an object exists in the pool that it is equal to the provided \c obj, then the existing object is
  /// returned. Otherwise, a new entry is added to the pool and returned.
  config_ptr<T> create(const T& obj)
  {
    for (const T& existing_obj : objs) {
      if (existing_obj == obj) {
        return config_ptr<T>{existing_obj};
      }
    }
    objs.push_back(obj);
    return config_ptr<T>{objs.back()};
  }

private:
  // Note: We use a deque to maintain pointer validity.
  std::deque<T> objs;
};

} // namespace srsran
