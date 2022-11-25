/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "downlink_processor_single_executor_impl.h"
#include "srsgnb/phy/upper/upper_phy_rg_gateway.h"
#include "srsgnb/support/executors/task_executor.h"

using namespace srsgnb;

downlink_processor_single_executor_impl::downlink_processor_single_executor_impl(
    upper_phy_rg_gateway&             gateway_,
    std::unique_ptr<pdcch_processor>  pdcch_proc_,
    std::unique_ptr<pdsch_processor>  pdsch_proc_,
    std::unique_ptr<ssb_processor>    ssb_proc_,
    std::unique_ptr<csi_rs_processor> csi_rs_proc_,
    task_executor&                    executor_) :
  gateway(gateway_),
  current_grid(nullptr),
  pdcch_proc(std::move(pdcch_proc_)),
  pdsch_proc(std::move(pdsch_proc_)),
  ssb_proc(std::move(ssb_proc_)),
  csi_rs_proc(std::move(csi_rs_proc_)),
  executor(executor_),
  pending_pdus(0),
  is_send_allowed(false)
{
  srsgnb_assert(pdcch_proc, "Invalid PDCCH processor received.");
  srsgnb_assert(pdsch_proc, "Invalid PDSCH processor received.");
  srsgnb_assert(ssb_proc, "Invalid SSB processor received.");
  srsgnb_assert(csi_rs_proc, "Invalid CSI-RS processor received.");
}

void downlink_processor_single_executor_impl::process_pdcch(const pdcch_processor::pdu_t& pdu)
{
  if (current_grid == nullptr) {
    return;
  }

  increase_pending_pdus();

  executor.execute([this, pdu]() {
    pdcch_proc->process(*current_grid, pdu);

    decrease_pending_pdus_and_try_sending_grid();
  });
}

void downlink_processor_single_executor_impl::process_pdsch(
    const static_vector<span<const uint8_t>, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS>& data,
    const pdsch_processor::pdu_t&                                                        pdu)
{
  if (current_grid == nullptr) {
    return;
  }

  increase_pending_pdus();

  executor.execute([this, data, pdu]() {
    pdsch_proc->process(*current_grid, data, pdu);

    decrease_pending_pdus_and_try_sending_grid();
  });
}

void downlink_processor_single_executor_impl::process_ssb(const ssb_processor::pdu_t& pdu)
{
  if (current_grid == nullptr) {
    return;
  }

  increase_pending_pdus();

  executor.execute([this, pdu]() {
    ssb_proc->process(pdu, *current_grid);

    decrease_pending_pdus_and_try_sending_grid();
  });
}

void downlink_processor_single_executor_impl::process_nzp_csi_rs(const csi_rs_processor::config_t& config)
{
  if (current_grid == nullptr) {
    return;
  }

  increase_pending_pdus();

  executor.execute([this, config]() {
    csi_rs_proc->map(*current_grid, config);

    decrease_pending_pdus_and_try_sending_grid();
  });
}

void downlink_processor_single_executor_impl::configure_resource_grid(const resource_grid_context& context,
                                                                      resource_grid&               grid)
{
  {
    std::lock_guard<std::mutex> lock(mutex);
    is_send_allowed = false;
    srsgnb_assert(pending_pdus == 0, "Reusing downlink processor that it is still processing PDUs.");
  }

  rg_context   = context;
  current_grid = &grid;
}

void srsgnb::downlink_processor_single_executor_impl::finish_processing_pdus()
{
  {
    std::lock_guard<std::mutex> lock(mutex);
    // No more slot messages will be received.
    is_send_allowed = true;
  }

  // Send the grid if all the PDUs finished to process.
  handle_resource_grid_send_opportunity();
}

void downlink_processor_single_executor_impl::handle_resource_grid_send_opportunity()
{
  std::lock_guard<std::mutex> lock(mutex);
  if (pending_pdus == 0 && is_send_allowed && current_grid) {
    gateway.send(rg_context, *current_grid);

    is_send_allowed = false;
    current_grid    = nullptr;
  }
}

void downlink_processor_single_executor_impl::increase_pending_pdus()
{
  std::lock_guard<std::mutex> lock(mutex);
  ++pending_pdus;
}

void downlink_processor_single_executor_impl::decrease_pending_pdus_and_try_sending_grid()
{
  {
    std::lock_guard<std::mutex> lock(mutex);
    --pending_pdus;
  }

  handle_resource_grid_send_opportunity();
}
