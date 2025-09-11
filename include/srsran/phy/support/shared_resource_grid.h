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

#include "srsran/phy/support/resource_grid.h"
#include "srsran/support/srsran_assert.h"
#include <utility>

namespace srsran {

class resource_grid_reader;
class resource_grid_writer;

/// \brief Unique resource grid.
///
/// It feedbacks to the resource grid pool the destruction of the instance.
class shared_resource_grid
{
public:
  /// Resource grid pool interface.
  class pool_interface
  {
  public:
    /// Default destructor.
    virtual ~pool_interface() = default;

    /// Gets the reference to the resource grid.
    virtual resource_grid& get() = 0;

    /// Notifies the pool of the release of the resource grid.
    virtual void notify_release_scope() = 0;
  };

  /// Default constructor creates an invalid resource grid.
  shared_resource_grid() = default;

  /// \brief Constructs a shared resource grid.
  /// \param pool_       Reference to the resource grid pool.
  /// \param ref_count_  Reference counter.
  shared_resource_grid(pool_interface& pool_, std::atomic<unsigned>& ref_count_) : pool(&pool_), ref_count(&ref_count_)
  {
  }

  /// \brief Move constructor - moves the ownership of other resource grid.
  ///
  /// Copies the pool reference and reference count. It invalidates the other instance.
  shared_resource_grid(shared_resource_grid&& other) noexcept :
    pool(std::exchange(other.pool, nullptr)), ref_count(std::exchange(other.ref_count, nullptr))
  {
  }

  /// Overload copy assign operator is forbade to avoid copying unintentionally.
  shared_resource_grid& operator=(const shared_resource_grid& other) = delete;

  /// \brief Overload move assign operator.
  ///
  /// It releases the current ownership and takes the ownership of the other instance.
  shared_resource_grid& operator=(shared_resource_grid&& other) noexcept
  {
    // First release previous grid.
    release();

    // Overwrite with the other grid.
    pool      = other.pool;
    ref_count = other.ref_count;

    // Invalidate the other grid.
    other.pool      = nullptr;
    other.ref_count = nullptr;

    return *this;
  }

  /// Default destructor - It releases the resource grid identifier.
  ~shared_resource_grid() { release(); }

  /// \brief Explicitly releases the resource grid.
  ///
  /// The last owner to release the grid shall notify the pool.
  void release()
  {
    if (is_valid()) {
      bool last = dec_ref_count();
      if (last) {
        pool->notify_release_scope();
      }
      pool      = nullptr;
      ref_count = nullptr;
    }
  }

  /// Gets the resource grid.
  resource_grid& get()
  {
    srsran_assert(is_valid(), "The resource grid is invalid.");
    return pool->get();
  }

  /// Gets the resource grid for read-only.
  resource_grid& get() const
  {
    srsran_assert(pool != nullptr, "The resource grid is invalid.");
    return pool->get();
  }

  /// Gets the resource grid reader.
  const resource_grid_reader& get_reader() const
  {
    srsran_assert(pool != nullptr, "The resource grid is invalid.");
    return pool->get().get_reader();
  }

  /// Gets the resource grid writer.
  resource_grid_writer& get_writer() const
  {
    srsran_assert(pool != nullptr, "The resource grid is invalid.");
    return pool->get().get_writer();
  }

  /// Explicit copy of the resource grid.
  shared_resource_grid copy() const { return shared_resource_grid(*this); }

  /// Accesses the resource grid pointer for read and write.
  resource_grid* operator->() { return &get(); }

  /// Accesses the resource grid pointer for read-only.
  const resource_grid& operator->() const { return get(); }

  /// Forbids reference operator to avoid changing the scope by reference.
  resource_grid* operator&() = delete;

  /// \brief Determines whether the resource grid is valid.
  ///
  /// \return \c true if the pool and the reference counter are not \c nullptr.
  bool is_valid() const { return (pool != nullptr) && (ref_count != nullptr); }

  /// \brief Overload conversion to bool.
  /// \return \c true if the resource grid is valid.
  explicit operator bool() const noexcept { return is_valid(); }

private:
  /// Copy constructor is explicit to avoid copying unintentionally.
  explicit shared_resource_grid(const shared_resource_grid& other) noexcept :
    pool(other.pool), ref_count(other.ref_count)
  {
    if (is_valid()) {
      inc_ref_count();
    }
  }

  /// \brief Decrement the reference counter.
  /// \return \c true if it is the last.
  /// \remark This method assumes the instance is valid.
  bool dec_ref_count() { return ref_count->fetch_sub(1, std::memory_order::memory_order_acq_rel) == 1; }

  /// \brief Increases the reference count by one.
  /// \remark This method assumes the instance is valid.
  void inc_ref_count() { ref_count->fetch_add(1, std::memory_order::memory_order_relaxed); }

  /// Resource grid pool. Set to \c nullptr for invalid resource grid.
  pool_interface* pool = nullptr;
  /// Reference counter. Set to \c nullptr for invalid resource grid.
  std::atomic<unsigned>* ref_count = nullptr;
};

} // namespace srsran
