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

#include "srsran/fapi_adaptor/mac/messages/pucch.h"
#include "srsran/mac/mac_cell_result.h"
#include "srsran/scheduler/scheduler_pucch_format.h"

using namespace srsran;
using namespace fapi_adaptor;

void srsran::fapi_adaptor::convert_pucch_mac_to_fapi(fapi::ul_pucch_pdu& fapi_pdu, const pucch_info& mac_pdu)
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
  const prb_interval&   hop_prbs            = mac_pdu.resources.second_hop_prbs;
  const pucch_format_1& f1                  = mac_pdu.format_1;
  const bool            intra_slot_freq_hop = hop_prbs.empty() ? false : true;
  builder.set_hopping_information_parameters(
      intra_slot_freq_hop, hop_prbs.start(), f1.group_hopping, f1.n_id_hopping, f1.initial_cyclic_shift);

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

/// Fills the Format 2 parameters.
static void fill_format2_parameters(fapi::ul_pucch_pdu_builder& builder, const pucch_info& mac_pdu)
{
  // Hopping parameters.
  const prb_interval&   hop_prbs            = mac_pdu.resources.second_hop_prbs;
  const pucch_format_2& f2                  = mac_pdu.format_2;
  const bool            intra_slot_freq_hop = hop_prbs.empty() ? false : true;
  builder.set_hopping_information_format2_parameters(intra_slot_freq_hop, hop_prbs.start());

  // Do not use pi/2 BPSK for UCI symbols.
  static const bool use_pi_to_bpsk = false;
  // Format 2 does not support multi slot repetition.
  pucch_repetition_tx_slot pucch_repetition = pucch_repetition_tx_slot::no_multi_slot;
  builder.set_common_parameters(mac_pdu.format, pucch_repetition, use_pi_to_bpsk);

  // Scrambling.
  builder.set_scrambling_parameters(f2.n_id_scambling);
  builder.set_format2_parameters(f2.n_id_0_scrambling);

  // Max coding rate.
  builder.set_maintenance_v3_basic_parameters({static_cast<unsigned>(f2.max_code_rate)}, {});

  // Bit lengths.
  builder.set_bit_length_parameters(convert_sr_bits_to_unsigned(f2.sr_bits), f2.harq_ack_nof_bits, f2.csi_part1_bits);
}

static void fill_custom_parameters(fapi::ul_pucch_pdu_builder& builder, const pucch_info& mac_pdu)
{
  switch (mac_pdu.format) {
    case pucch_format::FORMAT_1:
      fill_format1_parameters(builder, mac_pdu);
      break;
    case pucch_format::FORMAT_2:
      fill_format2_parameters(builder, mac_pdu);
      break;
    default:
      srsran_assert(0, "Invalid PUCCH format={}", mac_pdu.format);
  }
}

void srsran::fapi_adaptor::convert_pucch_mac_to_fapi(fapi::ul_pucch_pdu_builder& builder, const pucch_info& mac_pdu)
{
  // Handle is not supported.
  static const unsigned handle = 0;
  builder.set_basic_parameters(mac_pdu.crnti, handle);

  const bwp_configuration& bwp_cfg = *mac_pdu.bwp_cfg;
  builder.set_bwp_parameters(bwp_cfg.crbs.length(), bwp_cfg.crbs.start(), bwp_cfg.scs, bwp_cfg.cp);

  const prb_interval& freq_prbs = mac_pdu.resources.prbs;
  builder.set_allocation_in_frequency_parameters(freq_prbs.start(), freq_prbs.length());

  const ofdm_symbol_range& symbols = mac_pdu.resources.symbols;
  builder.set_allocation_in_time_parameters(symbols.start(), symbols.length());

  fill_custom_parameters(builder, mac_pdu);
}
