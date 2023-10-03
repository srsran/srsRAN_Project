/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <atomic>
#include <memory>
#include <vector>

namespace srsran {

/// \brief Concurrent pool of instances that are accessed depending on the thread.
/// \tparam Type Object type to access.
template <typename Type>
class concurrent_thread_local_object_pool
{
public:
  /// \brief Creates a concurrent instance pool from a list of instances.
  ///
  /// \remark An assertion is triggered if the list of instances is empty or if one of the elements is a \c nullptr.
  concurrent_thread_local_object_pool(std::vector<Type> pool_) : pool(std::move(pool_))
  {
    srsran_assert(!pool.empty(), "Pool is empty.");
    srsran_assert(std::none_of(pool.begin(), pool.end(), [](auto& element) { return !element; }),
                  "Invalid instance in pool.");
  }

  /// \brief Creates a concurrent instance pool with a specific number of elements.
  ///
  /// \tparam Args            Parameter pack used to construct the pool elements.
  /// \param[in] nof_elements Number of pool elements to instantiate.
  /// \param[in] args         Element creation arguments.
  template <typename... Args>
  concurrent_thread_local_object_pool(unsigned nof_elements, Args&&... args)
  {
    pool.reserve(nof_elements);
    for (unsigned i_elem = 0; i_elem != nof_elements; ++i_elem) {
      pool.emplace_back(std::forward<Args>(args)...);
    }
  }

  /// Gets an instance from the pool.
  Type& get()
  {
    static std::atomic<unsigned> global_count = {0};
    thread_local unsigned        thread_index = [this]() {
      unsigned id = global_count++;
      srsran_assert(
          id < pool.size(), "Insufficient number of elements, i.e., {}, for thread index {}.", pool.size(), id);
      return id;
    }();

    // Select codeblock processor.
    return pool[thread_index];
  }

  /// Gets the number of elements in the pool.
  std::size_t size() const { return pool.size(); }

private:
  std::vector<Type> pool;
};

} // namespace srsran
