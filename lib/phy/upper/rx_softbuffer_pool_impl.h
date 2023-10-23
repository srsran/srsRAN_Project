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

#pragma once

#include "rx_softbuffer_impl.h"
#include "srsran/adt/ring_buffer.h"
#include "srsran/adt/static_vector.h"
#include "srsran/phy/upper/codeblock_metadata.h"
#include "srsran/phy/upper/rx_softbuffer_pool.h"
#include "srsran/phy/upper/unique_rx_softbuffer.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/math_utils.h"
#include <mutex>

namespace srsran {

/// Describes a receiver softbuffer pool.
class rx_softbuffer_pool_impl : public rx_softbuffer_pool
{
private:
  /// Codeblock softbuffer pool.
  rx_softbuffer_codeblock_pool codeblock_pool;
  /// Storage of softbuffers.
  ring_buffer<std::unique_ptr<rx_softbuffer_impl>> available_buffers;
  ring_buffer<std::unique_ptr<rx_softbuffer_impl>> reserved_buffers;
  /// Indicates the lifetime of a softbuffer reservation as a number of slots.
  unsigned expire_timeout_slots;
  /// Protects methods from concurrent calls.
  std::mutex mutex;
  /// Logger.
  srslog::basic_logger& logger;

public:
  /// \brief Creates a generic receiver softbuffer pool.
  /// \param[in] config Provides the pool required parameters.
  rx_softbuffer_pool_impl(const rx_softbuffer_pool_config& config) :
    codeblock_pool(config.max_nof_codeblocks, config.max_codeblock_size, config.external_soft_bits),
    available_buffers(config.max_softbuffers),
    reserved_buffers(config.max_softbuffers),
    expire_timeout_slots(config.expire_timeout_slots),
    logger(srslog::fetch_basic_logger("PHY", true))
  {
    for (unsigned i = 0, i_end = config.max_softbuffers; i != i_end; ++i) {
      available_buffers.push(std::make_unique<rx_softbuffer_impl>(codeblock_pool));
    }
  }

  // See interface for documentation.
  unique_rx_softbuffer
  reserve_softbuffer(const slot_point& slot, const rx_softbuffer_identifier& id, unsigned nof_codeblocks) override;

  // See interface for documentation.
  void run_slot(const slot_point& slot) override;
};

} // namespace srsran
