/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/phy/upper/channel_processors/ssb_processor.h"
#include "srsran/support/memory_pool/concurrent_thread_local_object_pool.h"

namespace srsran {

/// \brief Concurrent SSB processor pool.
///
/// Wraps a concurrent pool of SSB processors. It assigns a SSB processor to each thread that processes SSB.
class ssb_processor_pool : public ssb_processor
{
public:
  /// SSB processor pool type.
  using ssb_processor_pool_type = concurrent_thread_local_object_pool<ssb_processor>;

  /// Creates a SSB processor pool from a list of processors. Ownership is transferred to the pool.
  explicit ssb_processor_pool(std::shared_ptr<ssb_processor_pool_type> processors_) : processors(std::move(processors_))
  {
    srsran_assert(processors, "Invalid processor pool.");
  }

  // See interface for documentation.
  void process(resource_grid_writer& grid, const pdu_t& pdu) override
  {
    ssb_processor& processor = processors->get();
    return processor.process(grid, pdu);
  }

private:
  std::shared_ptr<ssb_processor_pool_type> processors;
};

} // namespace srsran
