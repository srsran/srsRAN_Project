/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

upper_phy_rx_symbol_handler_impl::upper_phy_rx_symbol_handler_impl(
    uplink_processor_pool&         ul_processor_pool_,
    uplink_slot_pdu_repository&    ul_pdu_repository_,
    rx_buffer_pool&                rm_buffer_pool_,
    upper_phy_rx_results_notifier& rx_results_notifier_) :
  ul_processor_pool(ul_processor_pool_),
  ul_pdu_repository(ul_pdu_repository_),
  rm_buffer_pool(rm_buffer_pool_),
  rx_results_notifier(rx_results_notifier_)
{
}

void upper_phy_rx_symbol_handler_impl::handle_rx_symbol(const upper_phy_rx_symbol_context& context,
                                                        const resource_grid_reader&        grid)
{
  // Run PUSCH buffer housekeeping when a new slot starts.
  if (context.symbol == 0) {
    rm_buffer_pool.run_slot(context.slot);
  }

  // Pull PDUs that their time allocation is complete for this symbol index.
  span<const uplink_slot_pdu_entry> pdus = ul_pdu_repository.get_pdus(context.slot, context.symbol);
  if (pdus.empty()) {
    return;
  }

  // Get the uplink processor.
  uplink_processor& ul_processor = ul_processor_pool.get_processor(context.slot, context.sector);

  // Process all the PDUs taken from the repository.
  for (const auto& pdu : pdus) {
    if (variant_holds_alternative<uplink_processor::pusch_pdu>(pdu)) {
      const auto& pusch_pdu = variant_get<uplink_processor::pusch_pdu>(pdu);
      process_pusch(pusch_pdu, ul_processor, grid, context.slot);
    } else if (variant_holds_alternative<uplink_processor::pucch_pdu>(pdu)) {
      const auto& pucch_pdu = variant_get<uplink_processor::pucch_pdu>(pdu);
      ul_processor.process_pucch(rx_results_notifier, grid, pucch_pdu);
    } else if (variant_holds_alternative<uplink_processor::srs_pdu>(pdu)) {
      const auto& srs_pdu = variant_get<uplink_processor::srs_pdu>(pdu);
      ul_processor.process_srs(rx_results_notifier, grid, srs_pdu);
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

  // Extract new data flag.
  bool new_data = proc_pdu.codeword->new_data;

  // Reserve receive buffer.
  unique_rx_buffer buffer = rm_buffer_pool.reserve(slot, id, nof_codeblocks, new_data);

  // Skip processing if the buffer is not valid. The pool shall log the context and the reason of the failure.
  if (!buffer.is_valid()) {
    // Report data-related discarded result if shared channel data is present.
    if (pdu.pdu.codeword.has_value()) {
      ul_pusch_results_data discarded_results =
          ul_pusch_results_data::create_discarded(to_rnti(pdu.pdu.rnti), pdu.pdu.slot, pdu.harq_id);
      rx_results_notifier.on_new_pusch_results_data(discarded_results);
    }

    // Report control-related discarded result if HARQ-ACK feedback is present.
    if (pdu.pdu.uci.nof_harq_ack > 0) {
      ul_pusch_results_control discarded_results =
          ul_pusch_results_control::create_discarded(to_rnti(pdu.pdu.rnti), pdu.pdu.slot, pdu.pdu.uci.nof_harq_ack);
      rx_results_notifier.on_new_pusch_results_control(discarded_results);
    }

    return;
  }

  // Retrieves transport block data and starts PUSCH processing.
  auto payload = rx_payload_pool.acquire_payload_buffer(pdu.tb_size);
  ul_processor.process_pusch(payload, std::move(buffer), rx_results_notifier, grid, pdu);
}
