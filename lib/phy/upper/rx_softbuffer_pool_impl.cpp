/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rx_softbuffer_pool_impl.h"

using namespace srsgnb;
rx_softbuffer* rx_softbuffer_pool_impl::reserve_softbuffer(const slot_point&               slot,
                                                           const rx_softbuffer_identifier& id,
                                                           unsigned int                    nof_codeblocks)
{
  std::unique_lock<std::mutex> lock(mutex);

  // Look for the same identifier.
  for (rx_softbuffer_impl& buffer : buffers) {
    if (buffer.match_id(id)) {
      slot_point expire_slot = slot + expire_timeout_slots;

      // Rserve buffer.
      buffer.reserve(id, expire_slot, nof_codeblocks);

      // If the reservation failed, return nullptr.
      if (!buffer.is_reserved()) {
        return nullptr;
      }

      return &buffer;
    }
  }

  // If the same identifier was not found, select the first available.
  for (rx_softbuffer_impl& buffer : buffers) {
    if (!buffer.is_reserved()) {
      slot_point expire_slot = slot + expire_timeout_slots;
      buffer.reserve(id, expire_slot, nof_codeblocks);

      // If the reservation failed, return nullptr.
      if (!buffer.is_reserved()) {
        return nullptr;
      }

      return &buffer;
    }
  }

  // If no available buffer is found...
  return nullptr;
}

void rx_softbuffer_pool_impl::free_softbuffer(const rx_softbuffer_identifier& id)
{
  std::unique_lock<std::mutex> lock(mutex);

  // Search for a buffer that matches the id.
  for (rx_softbuffer_impl& buffer : buffers) {
    if (buffer.match_id(id)) {
      // Free and early return.
      buffer.free();
      return;
    }
  }
}

void rx_softbuffer_pool_impl::run_slot(const slot_point& slot)
{
  std::unique_lock<std::mutex> lock(mutex);

  // Run slot for each buffer.
  for (rx_softbuffer_impl& buffer : buffers) {
    buffer.run_slot(slot);
  }
}

std::unique_ptr<rx_softbuffer_pool> srsgnb::create_rx_softbuffer_pool(const rx_softbuffer_pool_description& config)
{
  return std::make_unique<rx_softbuffer_pool_impl>(config);
}
