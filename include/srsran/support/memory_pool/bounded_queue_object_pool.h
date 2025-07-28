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

#ifdef ENABLE_TSAN
#include <sanitizer/tsan_interface.h>
#endif

#include "cameron314/concurrentqueue.h"
#include <memory>

namespace srsran {

/// \brief A bounded, thread-safe object pool designed for efficient reuse of large objects or instances of abstract
/// interfaces.
///
/// This template class manages a fixed number of reusable objects of type T. It minimizes dynamic memory allocation
/// overhead by reusing objects from a pre-allocated pool, which can be particularly beneficial in applications where
/// frequent allocations and deallocations are costly.
///
/// The object pool is thread-safe, allowing multiple threads to acquire and release objects concurrently without data
/// races.
///
/// \tparam T Type of the objects managed by the pool. Must be default constructible if using the constructor with a
/// specified number of elements.
template <typename T>
class bounded_queue_object_pool
{
  /// Custom deleter for \c std::unique_ptr that enqueues the object back into the pool.
  struct pool_deleter {
    pool_deleter() = default;
    pool_deleter(bounded_queue_object_pool& parent_) : parent(&parent_) {}
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

    bounded_queue_object_pool* parent;
  };

public:
  /// Alias for a unique pointer with custom deleter managing objects in the pool.
  using ptr = std::unique_ptr<T, pool_deleter>;

  /// \brief Constructs a \c bounded_object_pool from an existing collection of objects.
  ///
  /// This constructor initializes the pool with a span of pre-allocated unique pointers to objects of type \c T,
  /// enqueuing them into the internal queue for future use.
  ///
  /// \param objects_ A view containing initial \c unique_ptr<T> objects to populate the pool.
  bounded_queue_object_pool(span<std::unique_ptr<T>> objects_) :
    objects(objects_.size(), 0, std::thread::hardware_concurrency())
  {
    for (auto& obj : objects_) {
      report_error_if_not(objects.enqueue(std::move(obj)), "Failed to enqueue element.");
    }
  }

  /// \brief Constructs a \c bounded_object_pool with a specified number of elements.
  ///
  /// This constructor creates and enqueues a fixed number of objects into the pool using provided arguments for
  /// construction.
  ///
  /// \tparam Args Types of arguments forwarded to T's constructor.
  /// \param nof_elements Number of elements to create in the pool.
  /// \param args Arguments to forward to T's constructor for each object created.
  template <typename... Args>
  bounded_queue_object_pool(unsigned nof_elements, Args&&... args) :
    objects(nof_elements, 0, std::thread::hardware_concurrency())
  {
    for (unsigned i_elem = 0; i_elem != nof_elements; ++i_elem) {
      report_error_if_not(objects.enqueue(std::make_unique<T>(std::forward<Args>(args)...)),
                          "Failed to enqueue element.");
    }
  }

  /// \brief Retrieves an object from the pool if available.
  ///
  /// Attempts to dequeue an object from the internal queue. If successful, returns a smart pointer with custom deleter
  /// managing the object's lifecycle. Otherwise, returns \c nullptr indicating no objects are currently available.
  ///
  /// \return A \c unique_ptr<T, pool_deleter> owning the retrieved object, or \c nullptr if none is available.
  ptr get()
  {
    std::unique_ptr<T> popped;

    if (objects.try_dequeue(popped)) {
#ifdef ENABLE_TSAN
      __tsan_acquire((void*)popped.get());
#endif
      return ptr{popped.release(), pool_deleter{*this}};
    }

    return nullptr;
  }

private:
  /// Concurrent queue managing the storage and retrieval of pooled objects.
  moodycamel::ConcurrentQueue<std::unique_ptr<T>> objects;
};

} // namespace srsran
