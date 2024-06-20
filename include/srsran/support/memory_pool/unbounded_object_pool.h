/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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