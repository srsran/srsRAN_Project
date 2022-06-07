/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "downlink_processor_impl.h"
#include "srsgnb/phy/upper/upper_phy_rg_gateway.h"
#include "srsgnb/support/executors/task_executor.h"

using namespace srsgnb;

downlink_processor_impl::downlink_processor_impl(upper_phy_rg_gateway&            gateway,
                                                 std::unique_ptr<pdcch_processor> pdcch_proc_,
                                                 std::unique_ptr<pdsch_processor> pdsch_proc_,
                                                 std::unique_ptr<ssb_processor>   ssb_proc_,
                                                 task_executor&                   executor) :
  gateway(gateway),
  current_grid(nullptr),
  pdcch_proc(std::move(pdcch_proc_)),
  pdsch_proc(std::move(pdsch_proc_)),
  ssb_proc(std::move(ssb_proc_)),
  executor(executor),
  num_pdus(0),
  is_send_allowed(false)
{
  srsran_assert(pdcch_proc, "Invalid PDCCH processor received \n");
  srsran_assert(pdsch_proc, "Invalid PDSCH processor received \n");
  srsran_assert(ssb_proc, "Invalid SSB processor received \n");
}

void downlink_processor_impl::process_pdcch(pdcch_processor::pdu_t& pdu)
{
  {
    std::lock_guard<std::mutex> lock(mutex);
    ++num_pdus;
  }

  executor.execute([this, pdu]() mutable {
    pdcch_proc->process(*current_grid, pdu);
    {
      std::lock_guard<std::mutex> lock(mutex);
      --num_pdus;
    }

    send_grid();
  });
}

void downlink_processor_impl::process_pdsch(
    const static_vector<span<const uint8_t>, pdsch_processor::MAX_NOF_TRANSPORT_BLOCKS>& data,
    const pdsch_processor::pdu_t&                                                        pdu)
{
  {
    std::lock_guard<std::mutex> lock(mutex);
    ++num_pdus;
  }

  executor.execute([this, data, pdu]() {
    pdsch_proc->process(*current_grid, data, pdu);
    {
      std::lock_guard<std::mutex> lock(mutex);
      --num_pdus;
    }

    send_grid();
  });
}

void downlink_processor_impl::process_ssb(const ssb_processor::pdu_t& pdu)
{
  {
    std::lock_guard<std::mutex> lock(mutex);
    ++num_pdus;
  }

  executor.execute([this, pdu]() {
    ssb_proc->process(pdu, *current_grid);
    {
      std::lock_guard<std::mutex> lock(mutex);
      --num_pdus;
    }

    send_grid();
  });
}

void downlink_processor_impl::configure_resource_grid(const resource_grid_context& context, resource_grid& grid)
{
  {
    std::lock_guard<std::mutex> lock(mutex);
    is_send_allowed = false;
    srsran_assert(num_pdus == 0, "Reusing downlink processor that it is still processing PDUs");
  }

  rg_context   = context;
  current_grid = &grid;
}

void srsgnb::downlink_processor_impl::send_resource_grid()
{
  {
    std::lock_guard<std::mutex> lock(mutex);
    // No more slot messages will be received.
    is_send_allowed = true;
  }

  // Send the grid if all the PDUs finished to process.
  send_grid();
}

void downlink_processor_impl::send_grid()
{
  std::lock_guard<std::mutex> lock(mutex);
  if (num_pdus == 0 && is_send_allowed && current_grid) {
    gateway.send(rg_context, *current_grid);

    is_send_allowed = false;
    current_grid    = nullptr;
  }
}
