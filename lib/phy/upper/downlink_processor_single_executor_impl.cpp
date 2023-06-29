/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "downlink_processor_single_executor_impl.h"
#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/phy/upper/upper_phy_rg_gateway.h"
#include "srsran/support/executors/task_executor.h"

using namespace srsran;

downlink_processor_single_executor_impl::downlink_processor_single_executor_impl(
    upper_phy_rg_gateway&                 gateway_,
    std::unique_ptr<pdcch_processor>      pdcch_proc_,
    std::unique_ptr<pdsch_processor>      pdsch_proc_,
    std::unique_ptr<ssb_processor>        ssb_proc_,
    std::unique_ptr<nzp_csi_rs_generator> csi_rs_proc_,
    task_executor&                        executor_) :
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
  srsran_assert(pdcch_proc, "Invalid PDCCH processor received.");
  srsran_assert(pdsch_proc, "Invalid PDSCH processor received.");
  srsran_assert(ssb_proc, "Invalid SSB processor received.");
  srsran_assert(csi_rs_proc, "Invalid CSI-RS processor received.");
}

void downlink_processor_single_executor_impl::process_pdcch(const pdcch_processor::pdu_t& pdu)
{
  executor.execute([this, pdu]() {
    if (current_grid == nullptr) {
      return;
    }

    increase_pending_pdus();

    resource_grid_mapper& mapper = current_grid->get_mapper();

    pdcch_proc->process(mapper, pdu);

    decrease_pending_pdus_and_try_sending_grid();
  });
}

void downlink_processor_single_executor_impl::process_pdsch(
    const static_vector<span<const uint8_t>, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS>& data,
    const pdsch_processor::pdu_t&                                                        pdu)
{
  executor.execute([this, data, pdu]() {
    if (current_grid == nullptr) {
      return;
    }

    increase_pending_pdus();

    resource_grid_mapper& mapper = current_grid->get_mapper();
    pdsch_proc->process(mapper, data, pdu);

    decrease_pending_pdus_and_try_sending_grid();
  });
}

void downlink_processor_single_executor_impl::process_ssb(const ssb_processor::pdu_t& pdu)
{
  executor.execute([this, pdu]() {
    if (current_grid == nullptr) {
      return;
    }

    increase_pending_pdus();

    ssb_proc->process(current_grid->get_writer(), pdu);

    decrease_pending_pdus_and_try_sending_grid();
  });
}

void downlink_processor_single_executor_impl::process_nzp_csi_rs(const nzp_csi_rs_generator::config_t& config)
{
  executor.execute([this, config]() {
    if (current_grid == nullptr) {
      return;
    }

    increase_pending_pdus();

    resource_grid_mapper& mapper = current_grid->get_mapper();

    csi_rs_proc->map(mapper, config);

    decrease_pending_pdus_and_try_sending_grid();
  });
}

void downlink_processor_single_executor_impl::configure_resource_grid(const resource_grid_context& context,
                                                                      resource_grid&               grid)
{
  // Initialize the resource grid asynchronously.
  executor.execute([&context, &grid, this]() {
    {
      std::lock_guard<std::mutex> lock(mutex);

      // Check that the DL processor has finished processing any previous tasks.
      srsran_assert(current_grid == nullptr, "A previously configured resource grid is still in use.");
      srsran_assert(pending_pdus == 0, "Reusing downlink processor that it is still processing PDUs.");

      is_send_allowed = false;
    }

    rg_context   = context;
    current_grid = &grid;

    increase_pending_pdus();
    current_grid->set_all_zero();
    decrease_pending_pdus_and_try_sending_grid();
  });
}

void srsran::downlink_processor_single_executor_impl::finish_processing_pdus()
{
  executor.execute([this]() {
    {
      std::lock_guard<std::mutex> lock(mutex);
      // No more slot messages will be received.
      is_send_allowed = true;
    }

    // Send the grid if all the PDUs finished to process.
    handle_resource_grid_send_opportunity();
  });
}

void downlink_processor_single_executor_impl::handle_resource_grid_send_opportunity()
{
  std::lock_guard<std::mutex> lock(mutex);
  if (is_send_allowed && (pending_pdus == 0) && (current_grid != nullptr)) {
    gateway.send(rg_context, current_grid->get_reader());

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
