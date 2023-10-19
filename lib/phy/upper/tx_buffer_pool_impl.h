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

#include "tx_buffer_impl.h"
#include "srsran/adt/ring_buffer.h"
#include "srsran/adt/static_vector.h"
#include "srsran/phy/upper/codeblock_metadata.h"
#include "srsran/phy/upper/tx_buffer_pool.h"
#include "srsran/phy/upper/unique_tx_buffer.h"
#include "srsran/support/error_handling.h"
#include "srsran/support/math_utils.h"
#include <mutex>

namespace srsran {

/// Describes a receiver buffer pool.
class tx_buffer_pool_impl : public tx_buffer_pool
{
public:
  /// \brief Creates a generic receiver buffer pool.
  /// \param[in] config Provides the pool required parameters.
  tx_buffer_pool_impl(const tx_buffer_pool_config& config) :
    logger(srslog::fetch_basic_logger("PHY", true)),
    codeblock_pool(config.nof_codeblocks, config.max_codeblock_size, config.external_soft_bits),
    available_buffers(config.nof_buffers),
    reserved_buffers(config.nof_buffers),
    expire_timeout_slots(config.expire_timeout_slots)
  {
    for (unsigned i = 0, i_end = config.nof_buffers; i != i_end; ++i) {
      available_buffers.push(std::make_unique<tx_buffer_impl>(codeblock_pool));
    }
  }

  // See interface for documentation.
  unique_tx_buffer
  reserve_buffer(const slot_point& slot, const tx_buffer_identifier& id, unsigned nof_codeblocks) override;

  // See interface for documentation.
  unique_tx_buffer reserve_buffer(const slot_point& slot, unsigned nof_codeblocks) override;

  // See interface for documentation.
  void run_slot(const slot_point& slot) override;

private:
  /// General physical layer logger.
  srslog::basic_logger& logger;
  /// Codeblock buffer pool.
  tx_buffer_codeblock_pool codeblock_pool;
  /// Storage of available buffers.
  ring_buffer<std::unique_ptr<tx_buffer_impl>> available_buffers;
  /// Storage of reserved buffers.
  ring_buffer<std::unique_ptr<tx_buffer_impl>> reserved_buffers;
  /// Indicates the lifetime of a buffer reservation as a number of slots.
  unsigned expire_timeout_slots;
  /// Protects methods from concurrent calls.
  std::mutex mutex;
};

} // namespace srsran
