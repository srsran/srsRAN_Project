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

#include "srsran/phy/support/prach_buffer.h"
#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/phy/upper/uplink_processor.h"
#include "srsran/phy/upper/upper_phy_rx_results_notifier.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {

class task_executor;

/// \brief Uplink processor implementation with a task dispatcher per channel.
///
/// In this implementation, every task of a physical channel is processed by the same executor. When a task finishes,
/// the results are notified by the upper_phy_rx_results_notifier interface.
class uplink_processor_task_dispatcher : public uplink_processor
{
public:
  uplink_processor_task_dispatcher(std::unique_ptr<uplink_processor> processor_,
                                   task_executor&                    pucch_executor_,
                                   task_executor&                    pusch_executor_,
                                   task_executor&                    prach_executor_) :
    processor(std::move(processor_)),
    pucch_executor(pucch_executor_),
    pusch_executor(pusch_executor_),
    prach_executor(prach_executor_)
  {
    srsran_assert(processor, "A valid uplink processor must be provided");
  }

  // See interface for documentation.
  void process_prach(upper_phy_rx_results_notifier& notifier,
                     const prach_buffer&            buffer,
                     const prach_buffer_context&    context) override
  {
    prach_executor.execute(
        [&notifier, &buffer, context, this]() { processor->process_prach(notifier, buffer, context); });
  }

  // See interface for documentation.
  void process_pusch(span<uint8_t>                      data,
                     unique_rx_softbuffer               softbuffer,
                     upper_phy_rx_results_notifier&     notifier,
                     const resource_grid_reader&        grid,
                     const uplink_processor::pusch_pdu& pdu) override
  {
    pusch_executor.execute([data, softbuffer2 = std::move(softbuffer), &notifier, &grid, pdu, this]() mutable {
      processor->process_pusch(data, std::move(softbuffer2), notifier, grid, pdu);
    });
  }

  // See interface for documentation.
  void process_pucch(upper_phy_rx_results_notifier& notifier,
                     const resource_grid_reader&    grid,
                     const pucch_pdu&               pdu) override
  {
    pucch_executor.execute([&notifier, &grid, pdu, this]() { processor->process_pucch(notifier, grid, pdu); });
  }

private:
  /// Uplink processor detector.
  std::unique_ptr<uplink_processor> processor;
  /// Executor for the PUCCH tasks generated within this uplink processor.
  task_executor& pucch_executor;
  /// Executor for the PUSCH tasks generated within this uplink processor.
  task_executor& pusch_executor;
  /// Executor for the PRACH tasks generated within this uplink processor.
  task_executor& prach_executor;
};

} // namespace srsran
