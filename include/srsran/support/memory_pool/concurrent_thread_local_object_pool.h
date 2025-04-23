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

#include "srsran/support/error_handling.h"
#include <atomic>
#include <memory>
#include <thread>
#include <vector>

namespace srsran {

/// \brief Concurrent pool of instances that are accessed depending on the thread.
///
/// The pool contains a vector of instances that are assigned to threads the first time they call get().
///
/// \tparam Type Object pool type.
template <typename Type>
class concurrent_thread_local_object_pool
{
public:
  /// \brief Creates a concurrent instance pool from a list of instances.
  ///
  /// \remark An assertion is triggered if the list of instances is empty or if one of the elements is a \c nullptr.
  concurrent_thread_local_object_pool(std::vector<std::unique_ptr<Type>> instances_) :
    instances(std::move(instances_)), thread_ids(instances.size(), std::this_thread::get_id())
  {
    srsran_assert(!instances.empty(), "Pool is empty.");
    srsran_assert(std::none_of(instances.begin(), instances.end(), [](auto& element) { return !element; }),
                  "Invalid instance in pool.");
  }

  /// \brief Creates a concurrent instance pool with a specific number of elements.
  ///
  /// \tparam Args            Parameter pack used to construct the pool elements.
  /// \param[in] nof_elements Number of pool elements to instantiate.
  /// \param[in] args         Element creation arguments.
  template <typename... Args>
  concurrent_thread_local_object_pool(unsigned nof_elements, Args&&... args) :
    thread_ids(nof_elements, std::this_thread::get_id())
  {
    instances.reserve(nof_elements);
    for (unsigned i_elem = 0; i_elem != nof_elements; ++i_elem) {
      instances.emplace_back(std::make_unique<Type>(std::forward<Args>(args)...));
    }
  }

  /// \brief Gets an instance from the pool.
  ///
  /// The selected instance is fixed for the thread that calls the method.
  ///
  /// \remark A fatal error is reported if the number of threads trying to get an instance exceeds the number of
  /// instances.
  Type& get()
  {
    // Get this thread identifier.
    std::thread::id this_thread_id = std::this_thread::get_id();

    // Gets the current number of assigned instances.
    unsigned current_thread_count = thread_ids_count.load(std::memory_order_acquire);

    // Try to find this thread identifier within the assigned threads.
    auto found = std::find_if(thread_ids.begin(),
                              thread_ids.begin() + current_thread_count,
                              [this_thread_id](std::thread::id id) { return id == this_thread_id; });

    // Convert from an iterator to an index.
    unsigned idx = std::distance(thread_ids.begin(), found);

    // If this thread identifier is not found, try to assign an instance to it.
    if (idx == current_thread_count) {
      // Get the next instance index. A fatal error is thrown if the number of instances is exceeded.
      idx = thread_id_next_index.fetch_add(1);
      report_fatal_error_if_not(idx < thread_ids.size(), "Insufficient number of instances.");

      // Assign thread to an instance.
      thread_ids[idx] = this_thread_id;

      // Increment the count. It protects the thread identifiers from data races.
      for (unsigned expected = idx;
           !thread_ids_count.compare_exchange_weak(expected, idx + 1, std::memory_order_acq_rel);
           expected = idx) {
        std::this_thread::sleep_for(std::chrono::microseconds(10));
      }
    }

    // Return the assigned instance.
    return *instances[idx];
  }

  /// Gets the pool capacity.
  std::size_t capacity() const { return instances.size(); }

private:
  /// Instance pool.
  std::vector<std::unique_ptr<Type>> instances;
  /// Thread identifiers.
  std::vector<std::thread::id> thread_ids;
  /// Next thread identifier index position to assign.
  std::atomic<unsigned> thread_id_next_index = 0;
  /// Counts the number of assigned instances.
  std::atomic<unsigned> thread_ids_count = 0;
};

} // namespace srsran
