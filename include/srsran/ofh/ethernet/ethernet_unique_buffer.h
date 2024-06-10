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

#include "srsran/adt/span.h"
#include "srsran/support/units.h"

namespace srsran {
namespace ether {

namespace detail {

/// Base vtable for move/destroy operations over the object stored in "unique_rx_buffer".
class storage_helper
{
public:
  constexpr storage_helper()                     = default;
  virtual ~storage_helper()                      = default;
  virtual void move(void* src, void* dest) const = 0;
  virtual void destroy(void* src) const          = 0;
  virtual bool empty() const                     = 0;
};

/// Specialization of move/destroy operations for when the "unique_rx_buffer" stores an object in its internal storage.
template <typename T>
class storage_helper_impl : public storage_helper
{
public:
  constexpr storage_helper_impl() = default;
  void move(void* src, void* dest) const override
  {
    ::new (dest) T(std::move(*static_cast<T*>(src)));
    static_cast<T*>(src)->~T();
  }
  void destroy(void* src) const override { static_cast<T*>(src)->~T(); }
  bool empty() const override { return false; }
};

/// Specialization of move/destroy operations for when the "unique_rx_buffer" is empty.
class empty_storage : public storage_helper
{
public:
  constexpr empty_storage() = default;
  void move(void* src, void* dest) const override {}
  void destroy(void* src) const override {}
  bool empty() const override { return true; }
};

} // namespace detail

/// Receive buffer interface.
class rx_buffer
{
public:
  virtual ~rx_buffer() = default;

  /// Returns a span of bytes received from the NIC.
  virtual span<const uint8_t> data() const = 0;
};

/// Receive buffer wrapper enforcing unique ownership of the stored buffer.
class unique_rx_buffer
{
  static constexpr units::bytes             storage_capacity{32};
  static const inline detail::empty_storage empty_object;

  using storage_t = std::aligned_storage_t<storage_capacity.value(), alignof(std::max_align_t)>;

public:
  /// Default constructor initializes internal storage with an empty object.
  constexpr unique_rx_buffer() noexcept : storage_ptr(&empty_object), buffer_ptr(nullptr) {}

  /// Copy constructor and copy assignment are deleted.
  unique_rx_buffer(const unique_rx_buffer& other)      = delete;
  unique_rx_buffer& operator=(unique_rx_buffer& other) = delete;

  /// Constructor receives an R-value reference to an object implementing rx_buffer interface.
  template <typename T>
  unique_rx_buffer(T&& rx_buffer_wrapper) noexcept(std::is_nothrow_move_constructible<T>::value)
  {
    static_assert(sizeof(rx_buffer_wrapper) <= storage_capacity.value(),
                  "Pre-allocated storage does not have enough space to store passed rx buffer");

    const static detail::storage_helper_impl<T> helper{};
    storage_ptr = &helper;
    ::new (&buffer) T(std::forward<T>(rx_buffer_wrapper));
  }

  /// Move constructor.
  unique_rx_buffer(unique_rx_buffer&& other) noexcept : storage_ptr(other.storage_ptr)
  {
    other.storage_ptr = &empty_object;
    storage_ptr->move(&other.buffer, &buffer);
  }

  /// Returns the view over stored data buffer.
  span<const uint8_t> data() const
  {
    if (storage_ptr->empty()) {
      return {};
    }
    return static_cast<const rx_buffer*>(static_cast<void*>(&buffer))->data();
  }

  /// Default destructor.
  ~unique_rx_buffer() { storage_ptr->destroy(&buffer); };

private:
  const detail::storage_helper* storage_ptr;
  union {
    mutable storage_t buffer;
    void*             buffer_ptr;
  };
};

} // namespace ether
} // namespace srsran
