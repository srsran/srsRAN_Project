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

#include "srsran/ofh/ethernet/ethernet_unique_buffer.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {
namespace ether {

class ethernet_rx_buffer_pool;

/// Receive buffer wrapper dedicated for socket-based Ethernet receiver.
class ethernet_rx_buffer_impl : public rx_buffer
{
public:
  /// Constructor receives the reference to the pool, from which one buffer is requested.
  ethernet_rx_buffer_impl(ethernet_rx_buffer_pool& pool_, unsigned id);

  /// Destructor frees this buffer in the pool.
  ~ethernet_rx_buffer_impl();

  /// Copy constructor is deleted.
  ethernet_rx_buffer_impl(ethernet_rx_buffer_impl& /**/) = delete;

  /// Move assigment operator.
  ethernet_rx_buffer_impl& operator=(ethernet_rx_buffer_impl&& other) = delete;

  /// Move constructor.
  ethernet_rx_buffer_impl(ethernet_rx_buffer_impl&& other) noexcept;

  // See interface for documentation.
  span<const uint8_t> data() const override;

  /// Returns span of bytes for writing.
  span<uint8_t> storage();

  /// Sets the actually used size.
  void resize(unsigned used_size);

private:
  ethernet_rx_buffer_pool& pool;
  int                      id = -1;
  /// Using size instead of a span here to save space and to enable using this class inside a \c unique_task
  /// implementation exploiting small buffer vtable.
  unsigned size = 0;
};

} // namespace ether
} // namespace srsran
