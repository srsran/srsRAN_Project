/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
    bool success = prach_executor.execute(
        [&notifier, &buffer, context, this]() { processor->process_prach(notifier, buffer, context); });

    if (!success) {
      logger.warning(context.slot.sfn(), context.slot.slot_index(), "Failed to execute PRACH. Ignoring detection.");
    }
  }

  // See interface for documentation.
  void process_pusch(span<uint8_t>                      data,
                     unique_rx_buffer                   rm_buffer,
                     upper_phy_rx_results_notifier&     notifier,
                     const resource_grid_reader&        grid,
                     const uplink_processor::pusch_pdu& pdu) override
  {
    bool success =
        pusch_executor.execute([data, rm_buffer2 = std::move(rm_buffer), &notifier, &grid, pdu, this]() mutable {
          processor->process_pusch(data, std::move(rm_buffer2), notifier, grid, pdu);
        });

    if (!success) {
      logger.warning(pdu.pdu.slot.sfn(), pdu.pdu.slot.slot_index(), "Failed to execute PUSCH. Ignoring processing.");
    }
  }

  // See interface for documentation.
  void process_pucch(upper_phy_rx_results_notifier& notifier,
                     const resource_grid_reader&    grid,
                     const pucch_pdu&               pdu) override
  {
    bool success =
        pucch_executor.execute([&notifier, &grid, pdu, this]() { processor->process_pucch(notifier, grid, pdu); });
    if (!success) {
      logger.warning(
          pdu.context.slot.sfn(), pdu.context.slot.slot_index(), "Failed to execute PUCCH. Ignoring processing.");
    }
  }

private:
  /// Default logger.
  srslog::basic_logger& logger = srslog::fetch_basic_logger("PHY");
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
