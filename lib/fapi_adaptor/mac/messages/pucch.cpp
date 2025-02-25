/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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
#include "srsran/scheduler/result/pucch_info.h"

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

/// Fills the Format 0 parameters.
static void fill_format0_parameters(fapi::ul_pucch_pdu_builder& builder, const pucch_info& mac_pdu)
{
  // Hopping parameters.
  const prb_interval&   hop_prbs            = mac_pdu.resources.second_hop_prbs;
  const pucch_format_0& f0                  = std::get<pucch_format_0>(mac_pdu.format_params);
  const bool            intra_slot_freq_hop = hop_prbs.empty() ? false : true;
  builder.set_hopping_information_parameters(
      intra_slot_freq_hop, hop_prbs.start(), f0.group_hopping, f0.n_id_hopping, f0.initial_cyclic_shift);

  // Do not use pi/2 BPSK for UCI symbols.
  static constexpr bool use_pi_to_bpsk = false;
  // Format 0 does not support multi slot repetition.
  pucch_repetition_tx_slot pucch_repetition = pucch_repetition_tx_slot::no_multi_slot;
  builder.set_common_parameters(mac_pdu.format(), pucch_repetition, use_pi_to_bpsk);

  // Format 0 does not support CSI.
  static constexpr unsigned csi_part1_bit_length = 0U;
  builder.set_bit_length_parameters(
      convert_sr_bits_to_unsigned(mac_pdu.uci_bits.sr_bits), mac_pdu.uci_bits.harq_ack_nof_bits, csi_part1_bit_length);
}

/// Fills the Format 1 parameters.
static void fill_format1_parameters(fapi::ul_pucch_pdu_builder& builder, const pucch_info& mac_pdu)
{
  // Hopping parameters.
  const prb_interval&   hop_prbs            = mac_pdu.resources.second_hop_prbs;
  const pucch_format_1& f1                  = std::get<pucch_format_1>(mac_pdu.format_params);
  const bool            intra_slot_freq_hop = hop_prbs.empty() ? false : true;
  builder.set_hopping_information_parameters(
      intra_slot_freq_hop, hop_prbs.start(), f1.group_hopping, f1.n_id_hopping, f1.initial_cyclic_shift);

  // Do not use pi/2 BPSK for UCI symbols.
  static constexpr bool use_pi_to_bpsk = false;
  builder.set_common_parameters(mac_pdu.format(), f1.slot_repetition, use_pi_to_bpsk);

  // Time domain occasion.
  builder.set_format1_parameters(f1.time_domain_occ);

  // Format 1 does not support CSI.
  static constexpr unsigned csi_part1_bit_length = 0U;
  builder.set_bit_length_parameters(
      convert_sr_bits_to_unsigned(mac_pdu.uci_bits.sr_bits), mac_pdu.uci_bits.harq_ack_nof_bits, csi_part1_bit_length);
}

/// Fills the Format 2 parameters.
static void fill_format2_parameters(fapi::ul_pucch_pdu_builder& builder, const pucch_info& mac_pdu)
{
  // Hopping parameters.
  const prb_interval&   hop_prbs            = mac_pdu.resources.second_hop_prbs;
  const pucch_format_2& f2                  = std::get<pucch_format_2>(mac_pdu.format_params);
  const bool            intra_slot_freq_hop = hop_prbs.empty() ? false : true;
  builder.set_hopping_information_format2_parameters(intra_slot_freq_hop, hop_prbs.start());

  // Do not use pi/2 BPSK for UCI symbols.
  static const bool use_pi_to_bpsk = false;
  // Format 2 does not support multi slot repetition.
  pucch_repetition_tx_slot pucch_repetition = pucch_repetition_tx_slot::no_multi_slot;
  builder.set_common_parameters(mac_pdu.format(), pucch_repetition, use_pi_to_bpsk);

  // Scrambling.
  builder.set_scrambling_parameters(f2.n_id_scambling);
  builder.set_dmrs_scrambling(f2.n_id_0_scrambling);

  // Max coding rate.
  builder.set_maintenance_v3_basic_parameters({static_cast<unsigned>(f2.max_code_rate)}, {});

  // Bit lengths.
  builder.set_bit_length_parameters(convert_sr_bits_to_unsigned(mac_pdu.uci_bits.sr_bits),
                                    mac_pdu.uci_bits.harq_ack_nof_bits,
                                    mac_pdu.uci_bits.csi_part1_nof_bits);
}

/// Fills the Format 3 parameters.
static void fill_format3_parameters(fapi::ul_pucch_pdu_builder& builder, const pucch_info& mac_pdu)
{
  const pucch_format_3& f3 = std::get<pucch_format_3>(mac_pdu.format_params);

  // Hopping parameters.
  const prb_interval& hop_prbs            = mac_pdu.resources.second_hop_prbs;
  const bool          intra_slot_freq_hop = hop_prbs.empty() ? false : true;
  // Both FAPI parameters initialCyclicShift (ref. TS 38.211 6.3.2.2.2) and m0PucchDmrsCyclicShift (ref.
  // TS 38.211 6.4.1.3.3.1) map to the same value, which is always 0 for PUCCH Format 3.
  constexpr unsigned m0_format3 = 0;

  // Parameter initial_cyclic_shift is not applicable to PUCCH Format 3.
  builder.set_hopping_information_parameters(
      intra_slot_freq_hop, hop_prbs.start(), f3.group_hopping, f3.n_id_hopping, m0_format3);

  // Common parameters.
  builder.set_common_parameters(mac_pdu.format(), f3.slot_repetition, f3.pi_2_bpsk);

  // Scrambling.
  builder.set_scrambling_parameters(f3.n_id_scrambling);

  // DM-RS.
  builder.set_dmrs_parameters(f3.additional_dmrs, f3.n_id_0_scrambling, m0_format3);

  // Max coding rate.
  builder.set_maintenance_v3_basic_parameters({static_cast<unsigned>(f3.max_code_rate)}, {});

  // Bit lengths.
  builder.set_bit_length_parameters(convert_sr_bits_to_unsigned(mac_pdu.uci_bits.sr_bits),
                                    mac_pdu.uci_bits.harq_ack_nof_bits,
                                    mac_pdu.uci_bits.csi_part1_nof_bits);
}

/// Gets the cyclic shift index (m0) for PUCCH Format 4, as per TS 38.211 Table 6.4.1.3.3.1-1.
static unsigned get_pucch_format4_m0(unsigned occ_index)
{
  switch (occ_index) {
    case 0:
      return 0;
    case 1:
      return 6;
    case 2:
      return 3;
    case 3:
      return 9;
    default:
      return 0;
  }
}

/// Fills the Format 4 parameters.
static void fill_format4_parameters(fapi::ul_pucch_pdu_builder& builder, const pucch_info& mac_pdu)
{
  const pucch_format_4& f4 = std::get<pucch_format_4>(mac_pdu.format_params);

  // Hopping parameters.
  const prb_interval& hop_prbs            = mac_pdu.resources.second_hop_prbs;
  const bool          intra_slot_freq_hop = hop_prbs.empty() ? false : true;
  // Both FAPI parameters initialCyclicShift (ref. TS 38.211 6.3.2.2.2) and m0PucchDmrsCyclicShift
  // (ref. TS 38.211 6.4.1.3.3.1) map to the same value.
  const unsigned m0_format4 = get_pucch_format4_m0(f4.orthog_seq_idx);

  builder.set_hopping_information_parameters(
      intra_slot_freq_hop, hop_prbs.start(), f4.group_hopping, f4.n_id_hopping, m0_format4);

  // Common parameters.
  builder.set_common_parameters(mac_pdu.format(), f4.slot_repetition, f4.pi_2_bpsk);

  // Scrambling.
  builder.set_scrambling_parameters(f4.n_id_scrambling);

  // DM-RS.
  builder.set_dmrs_parameters(f4.additional_dmrs, f4.n_id_0_scrambling, m0_format4);

  // Specific format 4 parameters.
  builder.set_format4_parameters(f4.orthog_seq_idx, static_cast<uint8_t>(f4.n_sf_pucch_f4));

  // Max coding rate.
  builder.set_maintenance_v3_basic_parameters({static_cast<unsigned>(f4.max_code_rate)}, {});

  // Bit lengths.
  builder.set_bit_length_parameters(convert_sr_bits_to_unsigned(mac_pdu.uci_bits.sr_bits),
                                    mac_pdu.uci_bits.harq_ack_nof_bits,
                                    mac_pdu.uci_bits.csi_part1_nof_bits);
}

static void fill_custom_parameters(fapi::ul_pucch_pdu_builder& builder, const pucch_info& mac_pdu)
{
  switch (mac_pdu.format()) {
    case pucch_format::FORMAT_0:
      fill_format0_parameters(builder, mac_pdu);
      break;
    case pucch_format::FORMAT_1:
      fill_format1_parameters(builder, mac_pdu);
      break;
    case pucch_format::FORMAT_2:
      fill_format2_parameters(builder, mac_pdu);
      break;
    case pucch_format::FORMAT_3:
      fill_format3_parameters(builder, mac_pdu);
      break;
    case pucch_format::FORMAT_4:
      fill_format4_parameters(builder, mac_pdu);
      break;
    default:
      srsran_assert(0, "Invalid PUCCH format={}", fmt::underlying(mac_pdu.format()));
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
