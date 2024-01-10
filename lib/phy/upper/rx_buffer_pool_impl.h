/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "rx_buffer_codeblock_pool.h"
#include "rx_buffer_impl.h"
#include "srsran/phy/upper/rx_buffer_pool.h"
#include "srsran/phy/upper/unique_rx_buffer.h"
#include "srsran/ran/slot_point.h"
#include "srsran/srslog/logger.h"
#include "srsran/srslog/srslog.h"
#include <vector>

namespace srsran {

/// Implements a PUSCH rate matcher buffer pool.
class rx_buffer_pool_impl : public rx_buffer_pool
{
private:
  /// No expiration time value.
  slot_point null_expiration = slot_point();

  /// Codeblock buffer pool.
  rx_buffer_codeblock_pool codeblock_pool;
  /// Actual buffer pool.
  std::vector<rx_buffer_impl> buffers;
  /// \brief List of buffer identifiers.
  ///
  /// Maps buffer identifiers to buffers. Each position indicates the buffer identifier assign to each of the buffers.
  /// Buffers with \c trx_buffer_identifier::invalid() identifier are not reserved.
  std::vector<trx_buffer_identifier> identifiers;
  /// Tracks expiration times.
  std::vector<slot_point> expirations;

  /// Indicates the lifetime of a buffer reservation as a number of slots.
  unsigned expire_timeout_slots;
  /// Logger.
  srslog::basic_logger& logger;

public:
  /// \brief Creates a generic receiver buffer pool.
  /// \param[in] config Provides the pool required parameters.
  rx_buffer_pool_impl(const rx_buffer_pool_config& config) :
    codeblock_pool(config.max_nof_codeblocks, config.max_codeblock_size, config.external_soft_bits),
    buffers(config.nof_buffers, rx_buffer_impl(codeblock_pool)),
    identifiers(config.nof_buffers, trx_buffer_identifier::invalid()),
    expirations(config.nof_buffers, null_expiration),
    expire_timeout_slots(config.expire_timeout_slots),
    logger(srslog::fetch_basic_logger("PHY", true))
  {
  }

  // See interface for documentation.
  unique_rx_buffer reserve(const slot_point& slot, trx_buffer_identifier id, unsigned nof_codeblocks) override;

  // See interface for documentation.
  void run_slot(const slot_point& slot) override;
};

} // namespace srsran