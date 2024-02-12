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

#include "srsran/adt/concurrent_queue.h"
#include "srsran/support/error_handling.h"
#include <vector>

namespace srsran {

/// \brief Concurrent pool of instances that are accessed depending on the thread.
///
/// The pool contains a vector of instances that are assigned to threads the first time they call get(). The instances
/// are returned to the pool when the thread is terminated.
///
/// The main thread is a special case as it does not get deleted, in that case it is not returned to the pool.
///
/// \tparam Type Object pool type.
template <typename Type>
class concurrent_thread_local_object_pool
{
  /// Pool type.
  using queue_type = concurrent_queue<std::unique_ptr<Type>,
                                      concurrent_queue_policy::lockfree_mpmc,
                                      concurrent_queue_wait_policy::sleep>;

public:
  /// \brief Creates a concurrent instance pool from a list of instances.
  ///
  /// \remark An assertion is triggered if the list of instances is empty or if one of the elements is a \c nullptr.
  concurrent_thread_local_object_pool(std::vector<std::unique_ptr<Type>> instances) :
    main_thread_id(std::this_thread::get_id()), queue(instances.size())
  {
    srsran_assert(!instances.empty(), "Pool is empty.");
    srsran_assert(std::none_of(instances.begin(), instances.end(), [](auto& element) { return !element; }),
                  "Invalid instance in pool.");

    for (std::unique_ptr<Type>& instance : instances) {
      queue.try_push(std::move(instance));
    }
  }

  /// \brief Creates a concurrent instance pool with a specific number of elements.
  ///
  /// \tparam Args            Parameter pack used to construct the pool elements.
  /// \param[in] nof_elements Number of pool elements to instantiate.
  /// \param[in] args         Element creation arguments.
  template <typename... Args>
  concurrent_thread_local_object_pool(unsigned nof_elements, Args&&... args) : queue(nof_elements)
  {
    for (unsigned i_elem = 0; i_elem != nof_elements; ++i_elem) {
      bool success = queue.try_push(std::make_unique<Type>(std::forward<Args>(args)...));
      report_error_if_not(success, "Failed to push element into the queue.");
    }
  }

  /// Default destructor - It triggers a failure if any instance has not been returned to que pool.
  ~concurrent_thread_local_object_pool()
  {
    if (main_thread_ptr) {
      report_error_if_not(queue.try_push(std::move(main_thread_ptr)),
                          "Failed to push element into the queue (destructor).");
    }
    report_fatal_error_if_not(queue.size() == queue.capacity(), "Not all instances are returned to the pool.");
  }

  /// Custom deleter that moves the ownership of the pointer back into the pool.
  class custom_deleter
  {
  public:
    custom_deleter(queue_type& queue_) : queue(&queue_) {}

    void operator()(Type* ptr)
    {
      // Ignore pointer if it is invalid.
      if (ptr == nullptr) {
        return;
      }

      bool success = queue->push_blocking(std::unique_ptr<Type>(ptr));
      report_error_if_not(success, "Failed to push element into the queue (deleter).");
    }

  private:
    queue_type* queue;
  };

  /// Specify unique pointer with custom deleter.
  using unique_ptr = std::unique_ptr<Type, custom_deleter>;

  /// \brief Gets an instance from the pool.
  ///
  /// The selected instance is fixed for the thread that calls the method.
  Type& get()
  {
    // Checks if the thread is the main.
    if (std::this_thread::get_id() == main_thread_id) {
      if (!main_thread_ptr) {
        report_fatal_error_if_not(queue.try_pop(main_thread_ptr), "Insufficient number of instances (main thread).");
      }
      return *main_thread_ptr;
    }

    thread_local unique_ptr ptr = [this]() {
      std::unique_ptr<Type> std_ptr;
      report_fatal_error_if_not(queue.try_pop(std_ptr), "Insufficient number of instances.");

      unique_ptr custom_ptr(std_ptr.get(), custom_deleter(queue));

      std_ptr.release();

      return custom_ptr;
    }();

    return *ptr;
  }

  /// Gets the pool capacity.
  std::size_t capacity() const { return queue.capacity(); }

private:
  /// Main thread identifier.
  std::thread::id main_thread_id;
  /// Main thread pointer.
  std::unique_ptr<Type> main_thread_ptr;
  /// Actual pool.
  queue_type queue;
};

} // namespace srsran
