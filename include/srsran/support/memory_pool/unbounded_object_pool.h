/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#ifdef ENABLE_TSAN
#include "sanitizer/tsan_interface.h"
#endif

#include "cameron314/concurrentqueue.h"
#include <memory>

namespace srsran {

/// Unbounded, thread-safe object pool. Ideal for large objects. Use with caution.
template <typename T>
class unbounded_object_pool
{
  struct pool_deleter {
    pool_deleter() = default;
    pool_deleter(unbounded_object_pool& parent_) : parent(&parent_) {}
    void operator()(T* ptr)
    {
      if (ptr != nullptr) {
        std::unique_ptr<T> obj{ptr};
#ifdef ENABLE_TSAN
        __tsan_release((void*)obj.get());
#endif
        parent->objects.enqueue(std::move(obj));
      }
    }

    unbounded_object_pool* parent;
  };

public:
  using ptr = std::unique_ptr<T, pool_deleter>;

  unbounded_object_pool(unsigned initial_capacity)
  {
    for (unsigned i = 0; i != initial_capacity; ++i) {
      objects.enqueue(std::make_unique<T>());
    }
  }

  ptr get()
  {
    std::unique_ptr<T> popped;
    if (objects.try_dequeue(popped)) {
#ifdef ENABLE_TSAN
      __tsan_acquire((void*)popped.get());
#endif
      return ptr{popped.release(), pool_deleter{*this}};
    }
    return ptr{new T(), pool_deleter{*this}};
  }

  unsigned current_capacity_approx() const
  {
    return objects.size_approx();
  }

private:
  moodycamel::ConcurrentQueue<std::unique_ptr<T>> objects;
};

} // namespace srsran
