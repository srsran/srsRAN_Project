/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "rx_softbuffer_pool_impl.h"

using namespace srsran;

unique_rx_softbuffer rx_softbuffer_pool_impl::reserve_softbuffer(const slot_point&               slot,
                                                                 const rx_softbuffer_identifier& id,
                                                                 unsigned                        nof_codeblocks)
{
  std::unique_lock<std::mutex> lock(mutex);
  slot_point                   expire_slot = slot + expire_timeout_slots;

  // Look for the same identifier.
  for (auto& buffer : buffers) {
    if (buffer->match_id(id)) {
      // Reserve buffer.
      if (!buffer->reserve(id, expire_slot, nof_codeblocks)) {
        // If the reservation failed, return an invalid softbuffer.
        return unique_rx_softbuffer();
      }

      return unique_rx_softbuffer(*buffer);
    }
  }

  // If the same identifier was not found, select the first available.
  for (auto& buffer : buffers) {
    if (buffer->is_available()) {
      // If the reservation failed, return an invalid softbuffer.
      if (!buffer->reserve(id, expire_slot, nof_codeblocks)) {
        return unique_rx_softbuffer();
      }

      return unique_rx_softbuffer(*buffer);
    }
  }

  // If no available buffer is found return an invalid buffer.
  return unique_rx_softbuffer();
}

void rx_softbuffer_pool_impl::run_slot(const slot_point& slot)
{
  std::unique_lock<std::mutex> lock(mutex);

  // Run slot for each buffer.
  for (auto& buffer : buffers) {
    buffer->run_slot(slot);
  }
}

std::unique_ptr<rx_softbuffer_pool> srsran::create_rx_softbuffer_pool(const rx_softbuffer_pool_config& config)
{
  return std::make_unique<rx_softbuffer_pool_impl>(config);
}
