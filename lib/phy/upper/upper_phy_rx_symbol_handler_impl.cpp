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

#include "upper_phy_rx_symbol_handler_impl.h"
#include "upper_phy_rx_results_notifier_wrapper.h"
#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc.h"
#include "srsran/phy/upper/channel_processors/pusch/formatters.h"
#include "srsran/phy/upper/rx_buffer_pool.h"
#include "srsran/phy/upper/unique_rx_buffer.h"
#include "srsran/phy/upper/uplink_processor.h"
#include "srsran/support/error_handling.h"
#include <utility>

using namespace srsran;

upper_phy_rx_symbol_handler_impl::upper_phy_rx_symbol_handler_impl(uplink_processor_pool&         ul_processor_pool_,
                                                                   uplink_slot_pdu_repository&    ul_pdu_repository_,
                                                                   rx_buffer_pool&                rm_buffer_pool_,
                                                                   upper_phy_rx_results_notifier& rx_results_notifier_,
                                                                   srslog::basic_logger&          logger_) :
  ul_processor_pool(ul_processor_pool_),
  ul_pdu_repository(ul_pdu_repository_),
  rm_buffer_pool(rm_buffer_pool_),
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
    logger.warning(context.slot.sfn(),
                   context.slot.slot_index(),
                   "Received notification for processing an uplink slot, but no PUSCH/PUCCH PDUs are expected to be "
                   "processed in the slot.");
    return;
  }

  const uplink_slot_pdu_entry& first_entry = pdus.front();
  unsigned                     nof_symbols = 0;
  if (variant_holds_alternative<uplink_processor::pusch_pdu>(first_entry)) {
    const uplink_processor::pusch_pdu& pdu = variant_get<uplink_processor::pusch_pdu>(first_entry);
    nof_symbols                            = get_nsymb_per_slot(pdu.pdu.cp);
  } else {
    const uplink_processor::pucch_pdu& pdu = variant_get<uplink_processor::pucch_pdu>(first_entry);
    nof_symbols                            = get_nsymb_per_slot(get_cp(pdu));
  }
  unsigned last_symbol_id = nof_symbols - 1U;

  // Process the PDUs when all symbols of the slot have been received.
  if (context.symbol != last_symbol_id) {
    return;
  }

  // Get the uplink processor.
  uplink_processor& ul_processor = ul_processor_pool.get_processor(context.slot, context.sector);

  // Process all the PDUs from the pool.
  for (const auto& pdu : pdus) {
    if (variant_holds_alternative<uplink_processor::pusch_pdu>(pdu)) {
      const uplink_processor::pusch_pdu& pusch_pdu = variant_get<uplink_processor::pusch_pdu>(pdu);
      process_pusch(pusch_pdu, ul_processor, grid, context.slot);
    } else if (variant_holds_alternative<uplink_processor::pucch_pdu>(pdu)) {
      const uplink_processor::pucch_pdu& pucch_pdu = variant_get<uplink_processor::pucch_pdu>(pdu);
      ul_processor.process_pucch(rx_results_notifier, grid, pucch_pdu);
    }
  }

  // Run PUSCH buffer housekeeping.
  rm_buffer_pool.run_slot(context.slot);
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
  srsran_assert(proc_pdu.codeword.has_value(),
                "PUSCH PDU doesn't contain data. Currently, that mode is not supported.");

  // Create buffer identifier.
  trx_buffer_identifier id(proc_pdu.rnti, pdu.harq_id);

  // Determine the number of codeblocks from the TBS and base graph.
  unsigned nof_codeblocks = ldpc::compute_nof_codeblocks(pdu.tb_size.to_bits(), proc_pdu.codeword->ldpc_base_graph);

  // Reserve receive buffer.
  unique_rx_buffer buffer = rm_buffer_pool.reserve(slot, id, nof_codeblocks);

  // Skip processing if the buffer is not valid. The pool shall log the context and the reason of the failure.
  if (!buffer.is_valid()) {
    return;
  }

  // Retrieves transport block data and starts PUSCH processing.
  auto payload = rx_payload_pool.acquire_payload_buffer(pdu.tb_size);
  ul_processor.process_pusch(payload, std::move(buffer), rx_results_notifier, grid, pdu);
}
