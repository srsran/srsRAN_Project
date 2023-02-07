/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "rx_softbuffer_impl.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/upper/codeblock_metadata.h"
#include "srsgnb/phy/upper/rx_softbuffer_pool.h"
#include "srsgnb/phy/upper/unique_rx_softbuffer.h"
#include "srsgnb/support/error_handling.h"
#include "srsgnb/support/math_utils.h"
#include <mutex>

namespace srsgnb {

/// Describes a receiver softbuffer pool.
class rx_softbuffer_pool_impl : public rx_softbuffer_pool
{
private:
  /// Codeblock softbuffer pool.
  rx_softbuffer_codeblock_pool codeblock_pool;
  /// Storage of softbuffers.
  std::vector<std::unique_ptr<rx_softbuffer_impl>> buffers;
  /// Indicates the lifetime of a softbuffer reservation as a number of slots.
  unsigned expire_timeout_slots;
  /// Protects methods from concurrent calls.
  std::mutex mutex;

public:
  /// \brief Creates a generic receiver softbuffer pool.
  /// \param[in] config Provides the pool required parameters.
  rx_softbuffer_pool_impl(const rx_softbuffer_pool_config& config) :
    codeblock_pool(config.max_nof_codeblocks, config.max_codeblock_size),
    buffers(config.max_softbuffers),
    expire_timeout_slots(config.expire_timeout_slots)
  {
    for (auto& buffer : buffers) {
      buffer = std::make_unique<rx_softbuffer_impl>(codeblock_pool);
    }
  }

  // See interface for documentation.
  unique_rx_softbuffer
  reserve_softbuffer(const slot_point& slot, const rx_softbuffer_identifier& id, unsigned nof_codeblocks) override;

  // See interface for documentation.
  void run_slot(const slot_point& slot) override;
};

} // namespace srsgnb
