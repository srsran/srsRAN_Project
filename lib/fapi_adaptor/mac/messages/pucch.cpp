/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi_adaptor/mac/messages/pucch.h"
#include "srsgnb/mac/mac_cell_result.h"
#include "srsgnb/scheduler/scheduler_pucch_format.h"

using namespace srsgnb;
using namespace fapi_adaptor;

void srsgnb::fapi_adaptor::convert_pucch_mac_to_fapi(fapi::ul_pucch_pdu& fapi_pdu, const pucch_info& mac_pdu)
{
  fapi::ul_pucch_pdu_builder builder(fapi_pdu);

  convert_pucch_mac_to_fapi(builder, mac_pdu);
}

/// Returns the number of SR bits as unsigned.
static unsigned convert_sr_bits_to_unsigned(sr_nof_bits value)
{
  switch (value) {
    case sr_nof_bits::no_sr:
      return 0U;
    case sr_nof_bits::one:
      return 1U;
    case sr_nof_bits::two:
      return 2U;
    case sr_nof_bits::three:
      return 3U;
    case sr_nof_bits::four:
      return 4U;
  }
  return 0U;
}

/// Fills the Format 1 parameters.
static void fill_format1_parameters(fapi::ul_pucch_pdu_builder& builder, const pucch_info& mac_pdu)
{
  // Hopping parameters.
  const prb_interval&   hop_prbs = mac_pdu.resources.second_hop_prbs;
  const pucch_format_1& f1       = mac_pdu.format_1;
  builder.set_hopping_information_parameters(mac_pdu.resources.intra_slot_freq_hop,
                                             hop_prbs.start(),
                                             f1.group_hopping,
                                             f1.n_id_hopping,
                                             f1.initial_cyclic_shift);

  // Do not use pi/2 BPSK for UCI symbols.
  static const bool use_pi_to_bpsk = false;
  builder.set_common_parameters(mac_pdu.format, f1.slot_repetition, use_pi_to_bpsk);

  // Time domain occasion.
  builder.set_format1_parameters(f1.time_domain_occ);

  // Format 1 does not support CSI.
  static const unsigned csi_part1_bit_length = 0U;
  builder.set_bit_length_parameters(
      convert_sr_bits_to_unsigned(f1.sr_bits), f1.harq_ack_nof_bits, csi_part1_bit_length);
}

static void fill_custom_parameters(fapi::ul_pucch_pdu_builder& builder, const pucch_info& mac_pdu)
{
  switch (mac_pdu.format) {
    case pucch_format::FORMAT_1:
      fill_format1_parameters(builder, mac_pdu);
      break;
    default:
      srsgnb_assert(0, "Invalid PUCCH format= {}", mac_pdu.format);
  }
}

void srsgnb::fapi_adaptor::convert_pucch_mac_to_fapi(fapi::ul_pucch_pdu_builder& builder, const pucch_info& mac_pdu)
{
  // Handle is not supported.
  static const unsigned handle = 0;
  builder.set_basic_parameters(mac_pdu.crnti, handle);

  // :TODO: update CP to the RAN type when it becomes available.
  const bwp_configuration& bwp_cfg = *mac_pdu.bwp_cfg;
  builder.set_bwp_parameters(bwp_cfg.crbs.length(),
                             bwp_cfg.crbs.start(),
                             bwp_cfg.scs,
                             (bwp_cfg.cp_extended) ? fapi::cyclic_prefix_type::extended
                                                   : fapi::cyclic_prefix_type::normal);

  const prb_interval& freq_prbs = mac_pdu.resources.prbs;
  builder.set_allocation_in_frequency_parameters(freq_prbs.start(), freq_prbs.length());

  const ofdm_symbol_range& symbols = mac_pdu.resources.symbols;
  builder.set_allocation_in_time_parameters(symbols.start(), symbols.length());

  fill_custom_parameters(builder, mac_pdu);
}
