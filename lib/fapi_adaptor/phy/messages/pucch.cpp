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

template <typename Config>
static void fill_common_parameters(Config& config, const fapi::ul_pucch_pdu& fapi_pdu, slot_point slot)
{
  config.slot         = slot;
  config.bwp_size_rb  = fapi_pdu.bwp_size;
  config.bwp_start_rb = fapi_pdu.bwp_start;
  config.cp           = fapi_pdu.cp;
  config.starting_prb = fapi_pdu.prb_start;
  if (fapi_pdu.intra_slot_frequency_hopping) {
    config.second_hop_prb.emplace(fapi_pdu.second_hop_prb);
  }

  config.ports = {0};
}

static void fill_format1_parameters(pucch_processor::format1_configuration& config,
                                    const fapi::ul_pucch_pdu&               fapi_pdu,
                                    slot_point                              slot)
{
  fill_common_parameters(config, fapi_pdu, slot);

  config.n_id         = fapi_pdu.nid_pucch_hopping;
  config.nof_harq_ack = fapi_pdu.bit_len_harq;

  config.initial_cyclic_shift = fapi_pdu.initial_cyclic_shift;
  config.nof_symbols          = fapi_pdu.nr_of_symbols;
  config.start_symbol_index   = fapi_pdu.start_symbol_index;
  config.time_domain_occ      = fapi_pdu.time_domain_occ_index;
}

/// Fills the context for Format 0 and Format 1.
static void fill_pucch_format_0_1_context(ul_pucch_context& context, const fapi::ul_pucch_pdu& fapi_pdu)
{
  context.context_f0_f1.emplace();
  ul_pucch_f0_f1_context& ctx = context.context_f0_f1.value();
  ctx.is_sr_opportunity       = fapi_pdu.sr_bit_len == 1U;
  ctx.nof_expected_harq_bits  = fapi_pdu.bit_len_harq;
}

void srsgnb::fapi_adaptor::convert_pucch_fapi_to_phy(uplink_processor::pucch_pdu& pdu,
                                                     const fapi::ul_pucch_pdu&    fapi_pdu,
                                                     uint16_t                     sfn,
                                                     uint16_t                     slot)
{
  // Context.
  ul_pucch_context& context = pdu.context;
  context.slot              = slot_point(fapi_pdu.scs, sfn, slot);
  context.rnti              = fapi_pdu.rnti;
  context.format            = fapi_pdu.format_type;
  if (context.format == pucch_format::FORMAT_0 || context.format == pucch_format::FORMAT_1) {
    fill_pucch_format_0_1_context(context, fapi_pdu);
  }

  switch (context.format) {
    case pucch_format::FORMAT_1:
      fill_format1_parameters(pdu.format1, fapi_pdu, slot_point(fapi_pdu.scs, sfn, slot));
      break;
    default:
      srsgnb_assert(0, "Current adaptor version only supports PUCCH format 1. Received format={}", context.format);
  }
}
