/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "upper_phy_rx_symbol_handler_impl.h"
#include "srsgnb/phy/support/prach_buffer_context.h"
#include "srsgnb/phy/support/resource_grid.h"
#include "srsgnb/phy/upper/uplink_processor.h"

using namespace srsgnb;

upper_phy_rx_symbol_handler_impl::upper_phy_rx_symbol_handler_impl(uplink_processor_pool&      ul_processor_pool,
                                                                   uplink_slot_pdu_repository& pdu_registry,
                                                                   rx_softbuffer_pool&         soft_pool,
                                                                   srslog::basic_logger&       logger) :
  ul_processor_pool(ul_processor_pool), pdu_repository(pdu_registry), soft_pool(soft_pool), logger(logger)
{
}

void upper_phy_rx_symbol_handler_impl::handle_rx_symbol(const upper_phy_rx_symbol_context& context,
                                                        const resource_grid_reader&        grid)
{
  // Check if all the symbols are present in the grid.
  span<const uplink_slot_pdu_entry> pdus        = pdu_repository.get_pdus(context.slot);
  unsigned                          nof_symbols = 0;

  if (pdus.empty()) {
    logger.error("No PDUs are registered to be processed within this slot {}", context.slot);
    return;
  }

  // :TODO: update this when adding PUCCH. First PDU can be a PUCCH PDU.
  nof_symbols             = get_nsymb_per_slot(pdus.front().pusch.pdu.cp);
  unsigned last_symbol_id = nof_symbols - 1U;

  if (context.symbol != last_symbol_id) {
    return;
  }

  // Get the uplink processor.
  uplink_processor& ul_processor = ul_processor_pool.get_processor(context.slot, context.sector);

  // Process all the PDUs from the pool.

  for (const auto& pdu : pdus) {
    switch (pdu.type) {
      case uplink_slot_pdu_entry::pdu_type::PUSCH: {
        process_pusch(pdu.pusch, ul_processor, grid, context.slot);
        break;
        case uplink_slot_pdu_entry::pdu_type::PUCCH:
          // :TODO: add PUCCH.
          break;
      }
    }
  }

  // Clear the PDUs from the repository once they are processed.
  pdu_repository.clear_slot(context.slot);
}

void upper_phy_rx_symbol_handler_impl::process_pusch(const uplink_processor::pusch_pdu& pdu,
                                                     uplink_processor&                  ul_processor,
                                                     const resource_grid_reader&        grid,
                                                     slot_point                         slot)
{
  const pusch_processor::pdu_t& proc_pdu = pdu.pdu;

  // Temporal sanity check as PUSCH is only supported for data. Remove the check when the UCI is supported for PUSCH.
  srsgnb_assert(proc_pdu.codeword.has_value(), "PUSCH PDU doesn't contain data. Currently, that mode is not supported");

  rx_softbuffer_identifier id;
  id.rnti        = static_cast<unsigned>(proc_pdu.rnti);
  id.harq_ack_id = pdu.harq_id;

  unsigned nof_codeblocks = ldpc::compute_nof_codeblocks(pdu.tb_size, proc_pdu.codeword->ldpc_base_graph);

  rx_softbuffer* buffer = soft_pool.reserve_softbuffer(slot, id, nof_codeblocks);
  if (!buffer) {
    logger.warning("Could not reserve a softbuffer for PUSCH PDU with RNTI={}, HARQ={} and slot={}",
                   id.rnti,
                   id.harq_ack_id,
                   proc_pdu.slot);
    return;
  }

  std::vector<uint8_t>& payload = payload_pool.get_next_container();
  payload.resize(pdu.tb_size);

  ul_processor.process_pusch(payload, *buffer, grid, pdu);
}

void upper_phy_rx_symbol_handler_impl::handle_rx_prach_window(const prach_buffer_context& context,
                                                              const prach_buffer&         buffer)
{
  // Get uplink processor.
  uplink_processor& ul_processor = ul_processor_pool.get_processor(context.slot, context.sector);

  // Process PRACH.
  ul_processor.process_prach(buffer, context);
}

void upper_phy_rx_symbol_handler_impl::handle_rx_srs_symbol(const upper_phy_rx_symbol_context& context)
{
  // :TODO: Implement me!!!
}
