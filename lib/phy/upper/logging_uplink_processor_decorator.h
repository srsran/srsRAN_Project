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
#include "srsgnb/phy/support/prach_buffer_context.h"
#include "srsgnb/phy/upper/uplink_processor.h"

namespace srsgnb {
class task_executor;

/// \brief Uplink processor implementation with a single executor.
///
/// In this implementation, every task is processed by the same executor. When a task finishes, the results are notified
/// by the upper_phy_rx_results_notifier interface.
class logging_uplink_processor_decorator : public uplink_processor
{
public:
  logging_uplink_processor_decorator(std::unique_ptr<uplink_processor> uplink_proc_, srslog::basic_logger& logger_) :
    uplink_proc(std::move(uplink_proc_)), logger(logger_)
  {
    srsgnb_assert(uplink_proc, "Invalid uplink processor.");
  }

  // See interface for documentation.
  void process_prach(upper_phy_rx_results_notifier& notifier,
                     const prach_buffer&            buffer,
                     const prach_buffer_context&    context) override
  {
    logger.set_context(context.slot.sfn(), context.slot.slot_index());
    uplink_proc->process_prach(notifier, buffer, context);
  }

  // See interface for documentation.
  void process_pusch(span<uint8_t>                      data,
                     unique_rx_softbuffer               softbuffer,
                     upper_phy_rx_results_notifier&     notifier,
                     const resource_grid_reader&        grid,
                     const uplink_processor::pusch_pdu& pdu) override
  {
    logger.set_context(pdu.pdu.slot.sfn(), pdu.pdu.slot.slot_index());
    uplink_proc->process_pusch(data, std::move(softbuffer), notifier, grid, pdu);
  }

  // See interface for documentation.
  void process_pucch(upper_phy_rx_results_notifier& notifier,
                     const resource_grid_reader&    grid,
                     const pucch_pdu&               pdu) override
  {
    logger.set_context(pdu.context.slot.sfn(), pdu.context.slot.slot_index());
    uplink_proc->process_pucch(notifier, grid, pdu);
  }

private:
  std::unique_ptr<uplink_processor> uplink_proc;
  srslog::basic_logger&             logger;
};
} // namespace srsgnb
