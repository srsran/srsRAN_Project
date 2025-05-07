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

#include "pusch_processor_validator_impl.h"
#include "pusch_processor_impl.h"
#include "srsran/ran/transform_precoding/transform_precoding_helpers.h"
#include "srsran/ran/uci/uci_formatters.h"

using namespace srsran;

pusch_processor_validator_impl::pusch_processor_validator_impl(
    const channel_estimate::channel_estimate_dimensions& ce_dims_) :
  ce_dims(ce_dims_)
{
  static constexpr interval<unsigned, true> nof_tx_layers_range(1, pusch_processor_impl::max_nof_layers);

  srsran_assert(
      (ce_dims.nof_prb > 0) && (ce_dims.nof_prb <= MAX_RB), "Invalid number of PRB (i.e. {}).", ce_dims.nof_prb);
  srsran_assert((ce_dims.nof_symbols > 0) && (ce_dims.nof_symbols <= MAX_NSYMB_PER_SLOT),
                "Invalid number of OFDM symbols.");
  srsran_assert(ce_dims.nof_rx_ports > 0, "The number of receive ports cannot be zero.");
  srsran_assert(nof_tx_layers_range.contains(ce_dims.nof_tx_layers),
                "Number of transmit layers (i.e. {}) is out of the range {}.",
                ce_dims.nof_tx_layers,
                nof_tx_layers_range);
}

error_type<std::string> pusch_processor_validator_impl::is_valid(const pusch_processor::pdu_t& pdu) const
{
  using namespace units::literals;
  unsigned nof_symbols_slot = get_nsymb_per_slot(pdu.cp);

  // The BWP size exceeds the grid size.
  if ((pdu.bwp_start_rb + pdu.bwp_size_rb) > ce_dims.nof_prb) {
    return make_unexpected(fmt::format(
        "The sum of the BWP start (i.e., {}) and size (i.e., {}) exceeds the maximum grid size (i.e., {} PRB).",
        pdu.bwp_start_rb,
        pdu.bwp_size_rb,
        ce_dims.nof_prb));
  }

  // The implementation only works with a single transmit layer.
  if (pdu.nof_tx_layers > ce_dims.nof_tx_layers) {
    return make_unexpected(fmt::format("The number of transmit layers (i.e., {}) is out of the range {}.",
                                       pdu.nof_tx_layers,
                                       interval<unsigned, true>(1, ce_dims.nof_tx_layers)));
  }

  // The number of receive ports cannot exceed the maximum dimensions.
  if (pdu.rx_ports.size() > ce_dims.nof_rx_ports) {
    return make_unexpected(
        fmt::format("The number of receive ports (i.e., {}) exceeds the maximum number of receive ports (i.e., {}).",
                    pdu.rx_ports.size(),
                    ce_dims.nof_rx_ports));
  }

  // The frequency allocation is not compatible with the BWP parameters.
  if (!pdu.freq_alloc.is_bwp_valid(pdu.bwp_start_rb, pdu.bwp_size_rb)) {
    return make_unexpected(
        fmt::format("Invalid BWP configuration, i.e., [{}, {}) for the given RB allocation, i.e., {}.",
                    pdu.bwp_start_rb,
                    pdu.bwp_start_rb + pdu.bwp_size_rb,
                    pdu.freq_alloc));
  }

  // Currently, none of the UCI field sizes can exceed 11 bit.
  static constexpr unsigned max_uci_len = 11;
  if ((pdu.uci.nof_harq_ack > max_uci_len) || (pdu.uci.nof_csi_part1 > max_uci_len)) {
    return make_unexpected(fmt::format("UCI field sizes in bits ({}, {}), exceed the maximum bit size, i.e., {}.",
                                       pdu.uci.nof_harq_ack,
                                       pdu.uci.nof_csi_part1,
                                       max_uci_len));
  }

  // CSI Part 2 size parameters must be compatible with the CSI Part 1 number of bits.
  if (!pdu.uci.csi_part2_size.is_valid(pdu.uci.nof_csi_part1)) {
    return make_unexpected(
        fmt::format("CSI Part 1 UCI field length (i.e., {}) does not correspond with the CSI Part 2 (i.e., {}).",
                    pdu.uci.nof_csi_part1,
                    pdu.uci.csi_part2_size));
  }

  // The limited buffer for rate matching size must not be zero.
  if (pdu.tbs_lbrm == 0_bytes) {
    return make_unexpected("Invalid LBRM size (0 bytes).");
  }

  // The number of OFDM symbols in the DM-RS mask must be equal to the number of OFDM symbols in a slot.
  if (pdu.dmrs_symbol_mask.size() != nof_symbols_slot) {
    return make_unexpected(
        fmt::format("The DM-RS symbol mask size (i.e., {}) must be equal to the slot size (i.e., {}).",
                    pdu.dmrs_symbol_mask.size(),
                    nof_symbols_slot));
  }

  // The number of symbols carrying DM-RS must be greater than zero.
  if (pdu.dmrs_symbol_mask.none()) {
    return make_unexpected("The number of OFDM symbols carrying DM-RS must be greater than zero.");
  }

  // The index of the first OFDM symbol carrying DM-RS shall be equal to or greater than the first symbol allocated to
  // transmission.
  int first_dmrs_symbol_index = pdu.dmrs_symbol_mask.find_lowest(true);
  if (static_cast<unsigned>(first_dmrs_symbol_index) < pdu.start_symbol_index) {
    return make_unexpected(fmt::format("The index of the first OFDM symbol carrying DM-RS (i.e., {}) must be equal to "
                                       "or greater than the first symbol allocated to transmission (i.e., {}).",
                                       first_dmrs_symbol_index,
                                       pdu.start_symbol_index));
  }

  // The index of the last OFDM symbol carrying DM-RS shall not be larger than the last symbol allocated to
  // transmission.
  int last_dmrs_symbol_index = pdu.dmrs_symbol_mask.find_highest(true);
  if (static_cast<unsigned>(last_dmrs_symbol_index) >= (pdu.start_symbol_index + pdu.nof_symbols)) {
    return make_unexpected(fmt::format("The index of the last OFDM symbol carrying DM-RS (i.e., {}) must be less than "
                                       "or equal to the last symbol allocated to transmission (i.e., {}).",
                                       last_dmrs_symbol_index,
                                       (pdu.start_symbol_index + pdu.nof_symbols - 1)));
  }

  // None of the occupied symbols must exceed the slot size.
  if (nof_symbols_slot < (pdu.start_symbol_index + pdu.nof_symbols)) {
    return make_unexpected(fmt::format("The symbol allocation (i.e., [{}, {})) exceeds the slot size (i.e., {}).",
                                       pdu.start_symbol_index,
                                       pdu.start_symbol_index + pdu.nof_symbols,
                                       nof_symbols_slot));
  }

  // Check if transform precoding is enabled.
  if (std::holds_alternative<pusch_processor::dmrs_configuration>(pdu.dmrs)) {
    const auto& dmrs_config = std::get<pusch_processor::dmrs_configuration>(pdu.dmrs);
    // Only DM-RS Type 1 is supported.
    if (dmrs_config.dmrs != dmrs_type::TYPE1) {
      return make_unexpected("Only DM-RS Type 1 is currently supported.");
    }

    // Only two CDM groups without data is supported.
    if (dmrs_config.nof_cdm_groups_without_data != 2) {
      return make_unexpected("Only two CDM groups without data are currently supported.");
    }
  } else {
    // Number of layers must be one.
    if (pdu.nof_tx_layers != 1) {
      return make_unexpected("Transform precoding is only possible with one layer.");
    }

    // Frequency allocation must be contiguous.
    if (!pdu.freq_alloc.is_contiguous()) {
      return make_unexpected("Transform precoding is only possible with contiguous allocations.");
    }

    // Number of PRB must be valid.
    if (!transform_precoding::is_nof_prbs_valid(pdu.freq_alloc.get_nof_rb())) {
      return make_unexpected("Transform precoding is only possible with a valid number of PRB.");
    }
  }

  // DC position is outside the channel estimate dimensions.
  interval<unsigned> dc_position_range(0, ce_dims.nof_prb * NRE);
  if (pdu.dc_position.has_value() && !dc_position_range.contains(*pdu.dc_position)) {
    return make_unexpected(
        fmt::format("DC position (i.e., {}) is out of range {}.", *pdu.dc_position, dc_position_range));
  }

  return default_success_t();
}
