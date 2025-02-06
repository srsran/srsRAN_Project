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
