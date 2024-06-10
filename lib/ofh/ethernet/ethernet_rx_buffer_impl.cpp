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

#include "ethernet_rx_buffer_impl.h"
#include "ethernet_rx_buffer_pool.h"

using namespace srsran;
using namespace ether;

ethernet_rx_buffer_impl::ethernet_rx_buffer_impl(ethernet_rx_buffer_pool& pool_, unsigned id_) : pool(pool_), id(id_)
{
  size = pool.get_data(id).size();
}

ethernet_rx_buffer_impl::ethernet_rx_buffer_impl(ethernet_rx_buffer_impl&& other) noexcept : pool(other.pool)
{
  id         = other.id;
  size       = other.size;
  other.id   = -1;
  other.size = 0;
}

span<const uint8_t> ethernet_rx_buffer_impl::data() const
{
  srsran_assert(id >= 0, "Invalid Ethernet rx buffer accessed");
  return pool.get_data(id).first(size);
}

span<uint8_t> ethernet_rx_buffer_impl::storage()
{
  srsran_assert(id >= 0, "Invalid Ethernet rx buffer accessed");
  return pool.get_data(id);
}

void ethernet_rx_buffer_impl::resize(unsigned used_size)
{
  srsran_assert(id >= 0, "Invalid Ethernet rx buffer accessed");
  srsran_assert(used_size <= pool.get_data(id).size(),
                "The size of buffer can not be bigger than the allocated size of {} bytes",
                pool.get_data(id).size());
  size = used_size;
}

ethernet_rx_buffer_impl::~ethernet_rx_buffer_impl()
{
  if (id >= 0) {
    pool.free(id);
  }
}
