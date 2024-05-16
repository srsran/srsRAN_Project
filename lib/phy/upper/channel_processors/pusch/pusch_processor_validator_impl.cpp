/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pusch_processor_validator_impl.h"
#include "pusch_processor_impl.h"

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

bool pusch_processor_validator_impl::is_valid(const pusch_processor::pdu_t& pdu) const
{
  unsigned nof_symbols_slot = get_nsymb_per_slot(pdu.cp);

  // The BWP size exceeds the grid size.
  if ((pdu.bwp_start_rb + pdu.bwp_size_rb) > ce_dims.nof_prb) {
    return false;
  }

  // The implementation only works with a single transmit layer.
  if (pdu.nof_tx_layers > ce_dims.nof_tx_layers) {
    return false;
  }

  // The number of receive ports cannot exceed the maximum dimensions.
  if (pdu.rx_ports.size() > ce_dims.nof_rx_ports) {
    return false;
  }

  // The frequency allocation is not compatible with the BWP parameters.
  if (!pdu.freq_alloc.is_bwp_valid(pdu.bwp_start_rb, pdu.bwp_size_rb)) {
    return false;
  }

  // Currently, none of the UCI field sizes can exceed 11 bit.
  static constexpr unsigned max_uci_len = 11;
  if ((pdu.uci.nof_harq_ack > max_uci_len) || (pdu.uci.nof_csi_part1 > max_uci_len)) {
    return false;
  }

  // CSI Part 2 must not be present if CSI Part 1 is not present.
  if ((pdu.uci.nof_csi_part1 == 0) && !pdu.uci.csi_part2_size.entries.empty()) {
    return false;
  }

  // CSI Part 2 size parameters must be compatible with the CSI Part 1 number of bits.
  if (!pdu.uci.csi_part2_size.is_valid(pdu.uci.nof_csi_part1)) {
    return false;
  }

  // The number of OFDM symbols in the DM-RS mask must be equal to the number of OFDM symbols in a slot.
  if (pdu.dmrs_symbol_mask.size() != nof_symbols_slot) {
    return false;
  }

  // The number of symbols carrying DM-RS must be greater than zero.
  if (pdu.dmrs_symbol_mask.none()) {
    return false;
  }

  // The index of the first OFDM symbol carrying DM-RS shall be equal to or greater than the first symbol allocated to
  // transmission.
  int first_dmrs_symbol_index = pdu.dmrs_symbol_mask.find_lowest(true);
  if (static_cast<unsigned>(first_dmrs_symbol_index) < pdu.start_symbol_index) {
    return false;
  }

  // The index of the last OFDM symbol carrying DM-RS shall not be larger than the last symbol allocated to
  // transmission.
  int last_dmrs_symbol_index = pdu.dmrs_symbol_mask.find_highest(true);
  if (static_cast<unsigned>(last_dmrs_symbol_index) >= (pdu.start_symbol_index + pdu.nof_symbols)) {
    return false;
  }

  // None of the occupied symbols must exceed the slot size.
  if (nof_symbols_slot < (pdu.start_symbol_index + pdu.nof_symbols)) {
    return false;
  }

  // Only DM-RS Type 1 is supported.
  if (pdu.dmrs != dmrs_type::TYPE1) {
    return false;
  }

  // Only two CDM groups without data is supported.
  if (pdu.nof_cdm_groups_without_data != 2) {
    return false;
  }

  // DC position is outside the channel estimate dimensions.
  interval<unsigned> dc_position_range(0, ce_dims.nof_prb * NRE);
  if (pdu.dc_position.has_value() && !dc_position_range.contains(pdu.dc_position.value())) {
    return false;
  }

  return true;
}
