/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once
#include "srsgnb/adt/circular_map.h"
#include "srsgnb/phy/upper/uplink_processor.h"
#include <thread>

namespace srsran {

class uplink_processor_concurrent : public uplink_processor
{
  using thread_id            = std::thread::id;
  using uplink_processor_ptr = std::unique_ptr<uplink_processor>;

public:
  class processor_pool
  {
  public:
    processor_pool(std::vector<std::unique_ptr<uplink_processor>> processors_) : processors(std::move(processors_)) {}

    uplink_processor& get_processor()
    {
      // Global count of threads.
      static std::atomic<unsigned> global_count = {0};
      // Local thread index.
      thread_local unsigned thread_index = global_count++;

      // Make sure the processor index does not exceed the number of processors.
      srsgnb_assert(thread_index < processors.size(), "Insufficient number of processors.");

      // Return the instance.
      return *processors[thread_index];
    }

  private:
    std::vector<uplink_processor_ptr> processors;
  };

  uplink_processor_concurrent(std::shared_ptr<processor_pool> pool_) : pool(std::move(pool_))
  {
    srsgnb_assert(pool, "Invalid pool.");
  }

  // See interface for documentation.
  void process_prach(upper_phy_rx_results_notifier& notifier,
                     const prach_buffer&            buffer,
                     const prach_buffer_context&    context) override
  {
    uplink_processor& processor = pool->get_processor();
    processor.process_prach(notifier, buffer, context);
  }

  // See interface for documentation.
  void process_pusch(span<uint8_t>                      data,
                     unique_rx_softbuffer               softbuffer,
                     upper_phy_rx_results_notifier&     notifier,
                     const resource_grid_reader&        grid,
                     const uplink_processor::pusch_pdu& pdu) override
  {
    uplink_processor& processor = pool->get_processor();
    processor.process_pusch(data, std::move(softbuffer), notifier, grid, pdu);
  }

  // See interface for documentation.
  void process_pucch(upper_phy_rx_results_notifier& notifier,
                     const resource_grid_reader&    grid,
                     const pucch_pdu&               pdu) override
  {
    uplink_processor& processor = pool->get_processor();
    processor.process_pucch(notifier, grid, pdu);
  }

private:
  // Internal processor pool.
  std::shared_ptr<processor_pool> pool;
};

} // namespace srsran
