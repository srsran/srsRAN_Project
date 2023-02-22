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
#include "srsran/phy/support/resource_grid_context.h"
#include "srsran/phy/upper/downlink_processor.h"

namespace srsran {
class task_executor;

/// \brief Downlink processor implementation with a single executor.
///
/// In this implementation, every task is processed by the same executor. When a task finishes, the results are notified
/// by the upper_phy_rx_results_notifier interface.
class logging_downlink_processor_decorator : public downlink_processor
{
public:
  logging_downlink_processor_decorator(std::unique_ptr<downlink_processor> downlink_proc_,
                                       srslog::basic_logger&               logger_) :
    downlink_proc(std::move(downlink_proc_)), logger(logger_)
  {
    srsran_assert(downlink_proc, "Invalid uplink processor.");
  }

  // See interface for documentation.
  void process_pdcch(const pdcch_processor::pdu_t& pdu) override { downlink_proc->process_pdcch(pdu); }

  // See interface for documentation.
  void process_pdsch(const static_vector<span<const uint8_t>, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS>& data,
                     const pdsch_processor::pdu_t&                                                        pdu) override
  {
    downlink_proc->process_pdsch(data, pdu);
  }

  // See interface for documentation.
  void process_ssb(const ssb_processor::pdu_t& pdu) override { downlink_proc->process_ssb(pdu); }

  // See interface for documentation.
  void process_nzp_csi_rs(const nzp_csi_rs_generator::config_t& config) override
  {
    downlink_proc->process_nzp_csi_rs(config);
  }

  // See interface for documentation.
  void configure_resource_grid(const resource_grid_context& context, resource_grid& grid) override
  {
    logger.set_context(context.slot.sfn(), context.slot.slot_index());

    downlink_proc->configure_resource_grid(context, grid);
  }

  // See interface for documentation.
  void finish_processing_pdus() override { downlink_proc->finish_processing_pdus(); }

  // See interface for documentation.
  bool is_reserved() const override { return downlink_proc->is_reserved(); }

private:
  std::unique_ptr<downlink_processor> downlink_proc;
  srslog::basic_logger&               logger;
};
} // namespace srsran
