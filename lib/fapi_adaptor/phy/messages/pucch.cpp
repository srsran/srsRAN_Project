/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi_adaptor/phy/messages/pucch.h"

using namespace srsgnb;
using namespace fapi_adaptor;

static void fill_format1_parameters(pucch_processor::format1_configuration& config,
                                    const fapi::ul_pucch_pdu&               fapi_pdu,
                                    slot_point                              slot)
{
  config.slot         = slot;
  config.bwp_size_rb  = fapi_pdu.bwp_size;
  config.bwp_start_rb = fapi_pdu.bwp_start;
  config.cp           = fapi_pdu.cp;
  config.starting_prb = fapi_pdu.prb_start;
  if (fapi_pdu.intra_slot_frequency_hopping) {
    config.second_hop_prb.emplace(fapi_pdu.second_hop_prb);
  }

  config.n_id         = fapi_pdu.nid_pucch_hopping;
  config.nof_harq_ack = fapi_pdu.bit_len_harq;

  config.ports = {0};

  config.initial_cyclic_shift = fapi_pdu.initial_cyclic_shift;
  config.nof_symbols          = fapi_pdu.nr_of_symbols;
  config.start_symbol_index   = fapi_pdu.start_symbol_index;
  config.time_domain_occ      = fapi_pdu.time_domain_occ_index;
}

static void fill_format2_parameters(pucch_processor::format2_configuration& config,
                                    const fapi::ul_pucch_pdu&               fapi_pdu,
                                    slot_point                              slot)
{
  config.slot         = slot;
  config.cp           = fapi_pdu.cp;
  config.ports        = {0};
  config.bwp_size_rb  = fapi_pdu.bwp_size;
  config.bwp_start_rb = fapi_pdu.bwp_start;
  config.starting_prb = fapi_pdu.prb_start;
  if (fapi_pdu.intra_slot_frequency_hopping) {
    config.second_hop_prb.emplace(fapi_pdu.second_hop_prb);
  }
  config.nof_prb            = fapi_pdu.prb_size;
  config.start_symbol_index = fapi_pdu.start_symbol_index;
  config.nof_symbols        = fapi_pdu.nr_of_symbols;

  config.rnti   = fapi_pdu.rnti;
  config.n_id   = fapi_pdu.nid_pucch_scrambling;
  config.n_id_0 = fapi_pdu.nid0_pucch_dmrs_scrambling;

  config.nof_harq_ack  = fapi_pdu.bit_len_harq;
  config.nof_sr        = fapi_pdu.sr_bit_len;
  config.nof_csi_part1 = fapi_pdu.csi_part1_bit_length;
  // CSI Part 2 disabled.
  config.nof_csi_part2 = 0;
}

/// Fills the context for Format 0 and Format 1.
static void fill_pucch_format_0_1_context(ul_pucch_context& context, const fapi::ul_pucch_pdu& fapi_pdu)
{
  ul_pucch_f0_f1_context& ctx = context.context_f0_f1.emplace();
  ctx.is_sr_opportunity       = fapi_pdu.sr_bit_len == 1U;
}

void srsgnb::fapi_adaptor::convert_pucch_fapi_to_phy(uplink_processor::pucch_pdu& pdu,
                                                     const fapi::ul_pucch_pdu&    fapi_pdu,
                                                     uint16_t                     sfn,
                                                     uint16_t                     slot)
{
  // Fill main context fields.
  ul_pucch_context& context = pdu.context;
  context.slot              = slot_point(fapi_pdu.scs, sfn, slot);
  context.rnti              = fapi_pdu.rnti;
  context.format            = fapi_pdu.format_type;
  // Format specific context fields.
  if (context.format == pucch_format::FORMAT_0 || context.format == pucch_format::FORMAT_1) {
    fill_pucch_format_0_1_context(context, fapi_pdu);
  }

  switch (context.format) {
    case pucch_format::FORMAT_1:
      fill_format1_parameters(pdu.format1, fapi_pdu, slot_point(fapi_pdu.scs, sfn, slot));
      break;
    case pucch_format::FORMAT_2:
      fill_format2_parameters(pdu.format2, fapi_pdu, slot_point(fapi_pdu.scs, sfn, slot));
      break;
    default:
      srsgnb_assert(0, "Unsupported PUCCH format {}", context.format);
  }
}
