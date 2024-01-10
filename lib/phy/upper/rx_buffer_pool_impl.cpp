/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "rx_buffer_pool_impl.h"
#include "rx_buffer_impl.h"
#include "srsran/phy/upper/rx_buffer_pool.h"
#include "srsran/phy/upper/unique_rx_buffer.h"
#include "srsran/ran/slot_point.h"
#include <algorithm>
#include <memory>

using namespace srsran;

unique_rx_buffer rx_buffer_pool_impl::reserve(const slot_point& slot, trx_buffer_identifier id, unsigned nof_codeblocks)
{
  // Try to find the HARQ identifier.
  auto id_found = std::find(identifiers.begin(), identifiers.end(), id);

  // Find an available buffer if no buffer was found with the same identifier.
  if (id_found == identifiers.end()) {
    id_found = std::find(identifiers.begin(), identifiers.end(), trx_buffer_identifier::invalid());
  }

  // Report warning and return invalid buffer if no available buffer has been found.
  if (id_found == identifiers.end()) {
    logger.warning(
        slot.sfn(), slot.slot_index(), "UL HARQ {}: failed to reserve, insufficient buffers in the pool.", id);
    return unique_rx_buffer();
  }

  // Get buffer index within the pool.
  unsigned i_buffer = id_found - identifiers.begin();

  // Get reference to the buffer.
  rx_buffer_impl& buffer = buffers[i_buffer];

  // Reset CRCs if one of the following conditions holds:
  // - The reservation identifier changed;
  // - The buffer is available; or
  // - The number of codeblocks changed.
  bool reset_crc = (identifiers[i_buffer] != id) || buffer.is_free() || (buffer.get_nof_codeblocks() != nof_codeblocks);

  // Reserve codeblocks.
  rx_buffer_status status = buffer.reserve(nof_codeblocks, reset_crc);

  // Report warning and return invalid buffer if the reservation is not successful.
  if (status != rx_buffer_status::successful) {
    logger.warning(slot.sfn(),
                   slot.slot_index(),
                   "UL HARQ {}: failed to reserve, {}.",
                   id,
                   (status == rx_buffer_status::already_in_use) ? "HARQ already in use" : "insufficient CBs");
    return unique_rx_buffer();
  }

  // Update identifier and expiration.
  identifiers[i_buffer] = id;
  expirations[i_buffer] = slot + expire_timeout_slots;

  // Create buffer.
  return unique_rx_buffer(buffer);
}

void rx_buffer_pool_impl::run_slot(const slot_point& slot)
{
  // Predicate for finding available buffers.
  auto pred = [](trx_buffer_identifier id) { return id != trx_buffer_identifier::invalid(); };

  // Iterate over all the buffers that are currently reserved.
  for (auto it = std::find_if(identifiers.begin(), identifiers.end(), pred); it != identifiers.end();
       it      = std::find_if(++it, identifiers.end(), pred)) {
    // Calculate buffer index.
    unsigned i_buffer = it - identifiers.begin();

    // Get reference to the buffer.
    rx_buffer_impl& buffer = buffers[i_buffer];

    // Determines whether the buffer is free.
    bool is_free = false;

    // A buffer is expired if the expiration slot is lower than or equal to the current slot.
    if ((expirations[i_buffer] != null_expiration) && (expirations[i_buffer] <= slot)) {
      // Try to expire the buffer.
      is_free = buffer.expire();

      // If the buffer is not available, increase the expiration time and continue to the next buffer.
      if (!is_free) {
        expirations[i_buffer] = slot + expire_timeout_slots;
        continue;
      }
    } else {
      // Check if the buffer is free before expiring.
      is_free = buffer.is_free();
    }

    // Clear identifier and expiration.
    if (is_free) {
      identifiers[i_buffer] = trx_buffer_identifier::invalid();
      expirations[i_buffer] = null_expiration;
    }
  }
}

std::unique_ptr<rx_buffer_pool> srsran::create_rx_buffer_pool(const rx_buffer_pool_config& config)
{
  return std::make_unique<rx_buffer_pool_impl>(config);
}