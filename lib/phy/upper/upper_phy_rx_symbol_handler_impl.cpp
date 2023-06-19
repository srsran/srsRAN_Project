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

#include "upper_phy_rx_symbol_handler_impl.h"
#include "upper_phy_rx_results_notifier_wrapper.h"
#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/phy/upper/unique_rx_softbuffer.h"
#include "srsran/phy/upper/uplink_processor.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

upper_phy_rx_symbol_handler_impl::upper_phy_rx_symbol_handler_impl(uplink_processor_pool&         ul_processor_pool_,
                                                                   uplink_slot_pdu_repository&    ul_pdu_repository_,
                                                                   rx_softbuffer_pool&            softbuffer_pool_,
                                                                   upper_phy_rx_results_notifier& rx_results_notifier_,
                                                                   srslog::basic_logger&          logger_) :
  ul_processor_pool(ul_processor_pool_),
  ul_pdu_repository(ul_pdu_repository_),
  softbuffer_pool(softbuffer_pool_),
  rx_results_notifier(rx_results_notifier_),
  logger(logger_)
{
}

/// Returns the cyclic prefix from the given uplink processor PUCCH PDU.
static cyclic_prefix get_cp(const uplink_processor::pucch_pdu& pdu)
{
  switch (pdu.context.format) {
    case pucch_format::FORMAT_0:
      return pdu.format0.cp;
    case pucch_format::FORMAT_1:
      return pdu.format1.cp;
    case pucch_format::FORMAT_2:
      return pdu.format2.cp;
    case pucch_format::FORMAT_3:
      return pdu.format3.cp;
    case pucch_format::FORMAT_4:
      return pdu.format4.cp;
    default:
      srsran_assert(0, "Invalid PUCCH format={}", pdu.context.format);
      break;
  }

  return cyclic_prefix::NORMAL;
}

void upper_phy_rx_symbol_handler_impl::handle_rx_symbol(const upper_phy_rx_symbol_context& context,
                                                        const resource_grid_reader&        grid)
{
  // Check if all the symbols are present in the grid.
  span<const uplink_slot_pdu_entry> pdus = ul_pdu_repository.get_pdus(context.slot);

  if (pdus.empty()) {
    logger.set_context(context.slot.sfn(), context.slot.slot_index());
    logger.warning("Received notification for processing an uplink slot, but no PUSCH/PUCCH PDUs are expected to be "
                   "processed in the slot.");
    return;
  }

  const uplink_slot_pdu_entry& first_entry = pdus.front();
  unsigned                     nof_symbols =
      get_nsymb_per_slot((first_entry.type == uplink_slot_pdu_entry::pdu_type::PUSCH) ? first_entry.pusch.pdu.cp
                                                                                      : get_cp(first_entry.pucch));
  unsigned last_symbol_id = nof_symbols - 1U;

  // Process the PDUs when all symbols of the slot have been received.
  if (context.symbol != last_symbol_id) {
    return;
  }

  // Get the uplink processor.
  uplink_processor& ul_processor = ul_processor_pool.get_processor(context.slot, context.sector);

  // Process all the PDUs from the pool.
  for (const auto& pdu : pdus) {
    switch (pdu.type) {
      case uplink_slot_pdu_entry::pdu_type::PUSCH:
        process_pusch(pdu.pusch, ul_processor, grid, context.slot);
        break;
      case uplink_slot_pdu_entry::pdu_type::PUCCH:
        ul_processor.process_pucch(rx_results_notifier, grid, pdu.pucch);
        break;
    }
  }
}

void upper_phy_rx_symbol_handler_impl::handle_rx_prach_window(const prach_buffer_context& context,
                                                              const prach_buffer&         buffer)
{
  // Get uplink processor.
  uplink_processor& ul_processor = ul_processor_pool.get_processor(context.slot, context.sector);

  // Process PRACH.
  ul_processor.process_prach(rx_results_notifier, buffer, context);
}

void upper_phy_rx_symbol_handler_impl::handle_rx_srs_symbol(const upper_phy_rx_symbol_context& context)
{
  report_fatal_error("upper_phy_rx_symbol_handler_impl::handle_rx_srs_symbol not yet implemented");
}

void upper_phy_rx_symbol_handler_impl::process_pusch(const uplink_processor::pusch_pdu& pdu,
                                                     uplink_processor&                  ul_processor,
                                                     const resource_grid_reader&        grid,
                                                     slot_point                         slot)
{
  const pusch_processor::pdu_t& proc_pdu = pdu.pdu;

  // Temporal sanity check as PUSCH is only supported for data. Remove the check when the UCI is supported for PUSCH.
  srsran_assert(proc_pdu.codeword.has_value(), "PUSCH PDU doesn't contain data. Currently, that mode is not supported");

  rx_softbuffer_identifier id;
  id.rnti        = static_cast<unsigned>(proc_pdu.rnti);
  id.harq_ack_id = pdu.harq_id;

  unsigned nof_codeblocks =
      ldpc::compute_nof_codeblocks(units::bytes(pdu.tb_size).to_bits(), proc_pdu.codeword->ldpc_base_graph);

  unique_rx_softbuffer buffer = softbuffer_pool.reserve_softbuffer(slot, id, nof_codeblocks);
  if (buffer.is_valid()) {
    auto payload = rx_payload_pool.acquire_payload_buffer(pdu.tb_size);
    ul_processor.process_pusch(payload, std::move(buffer), rx_results_notifier, grid, pdu);
    return;
  }

  logger.set_context(pdu.pdu.slot.sfn(), pdu.pdu.slot.slot_index());
  logger.warning("Could not reserve a softbuffer for PUSCH PDU with RNTI={}, HARQ={}", id.rnti, id.harq_ack_id);
}
