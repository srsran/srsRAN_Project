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

#include "pusch_processor_impl.h"
#include "pusch_decoder_buffer_dummy.h"
#include "pusch_processor_notifier_adaptor.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_codeword_buffer.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_buffer.h"
#include "srsran/phy/upper/unique_rx_buffer.h"
#include "srsran/ran/pusch/ulsch_info.h"
#include "srsran/ran/sch_dmrs_power.h"
#include "srsran/ran/uci/uci_formatters.h"
#include "srsran/ran/uci/uci_part2_size_calculator.h"

using namespace srsran;

namespace {
class pusch_processor_csi_part1_feedback_impl : public pusch_processor_csi_part1_feedback
{
public:
  pusch_processor_csi_part1_feedback_impl(pusch_uci_decoder_wrapper&        csi_part2_decoder_,
                                          ulsch_demultiplex&                demultiplex_,
                                          modulation_scheme                 modulation_,
                                          const uci_part2_size_description& csi_part2_size_,
                                          const ulsch_configuration&        ulsch_config_) :
    csi_part2_decoder(csi_part2_decoder_),
    demultiplex(demultiplex_),
    modulation(modulation_),
    csi_part2_size(csi_part2_size_),
    ulsch_config(ulsch_config_)
  {
  }

  void connect_notifier(pusch_processor_notifier_adaptor& notifier_) { notifier = &notifier_; }

  void on_csi_part1(const uci_payload_type& part1) override
  {
    srsran_assert(notifier != nullptr, "Notifier not connected.");

    unsigned nof_csi_part_2_bits = uci_part2_get_size(part1, csi_part2_size);

    // Skip if the number of CSI Part 2 bits is zero.
    if (nof_csi_part_2_bits == 0) {
      return;
    }

    // Update the number of CSI Part 2 bits.
    ulsch_config.nof_csi_part2_bits = units::bits(nof_csi_part_2_bits);

    // Recalculate the UL-SCH information.
    ulsch_information info = get_ulsch_information(ulsch_config);

    // Get CSI Part 2 notifier.
    pusch_uci_decoder_notifier& csi_part2_notifier = notifier->get_csi_part2_notifier();

    // Configure CSI Part 2 decoder.
    pusch_decoder_buffer& csi_part2_buffer =
        csi_part2_decoder.new_transmission(nof_csi_part_2_bits, modulation, csi_part2_notifier);

    // Configure UL-SCH demultiplex.
    demultiplex.set_csi_part2(csi_part2_buffer, nof_csi_part_2_bits, info.nof_csi_part2_bits.value());
  }

private:
  pusch_processor_notifier_adaptor* notifier;
  pusch_uci_decoder_wrapper&        csi_part2_decoder;
  ulsch_demultiplex&                demultiplex;
  modulation_scheme                 modulation;
  uci_part2_size_description        csi_part2_size;
  ulsch_configuration               ulsch_config;
};

} // namespace

// Dummy PUSCH decoder buffer. Used for PUSCH transmissions without SCH data.
static pusch_decoder_buffer_dummy decoder_buffer_dummy;

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

  // The number of symbols carrying DM-RS must be greater than zero.
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

pusch_processor_impl::pusch_processor_impl(configuration& config) :
  thread_local_dependencies_pool(std::move(config.thread_local_dependencies_pool)),
  decoder(std::move(config.decoder)),
  dec_nof_iterations(config.dec_nof_iterations),
  dec_enable_early_stop(config.dec_enable_early_stop),
  csi_sinr_calc_method(config.csi_sinr_calc_method)
{
  srsran_assert(thread_local_dependencies_pool, "Invalid dependency pool.");
  srsran_assert(decoder, "Invalid decoder.");
  srsran_assert(dec_nof_iterations != 0, "The decoder number of iterations must be non-zero.");
}

void pusch_processor_impl::process(span<uint8_t>                    data,
                                   unique_rx_buffer                 rm_buffer,
                                   pusch_processor_result_notifier& notifier,
                                   const resource_grid_reader&      grid,
                                   const pusch_processor::pdu_t&    pdu)
{
  using namespace units::literals;

  // Get thread local dependencies.
  concurrent_dependencies& depedencies = thread_local_dependencies_pool->get();

  // Get channel estimates.
  channel_estimate& ch_estimate = depedencies.get_channel_estimate();

  // Assert PDU.
  assert_pdu(pdu, ch_estimate);

  // Number of RB used by this transmission.
  unsigned nof_rb = pdu.freq_alloc.get_nof_rb();

  // Get RB mask relative to Point A. It assumes PUSCH is never interleaved.
  bounded_bitset<MAX_RB> rb_mask = pdu.freq_alloc.get_prb_mask(pdu.bwp_start_rb, pdu.bwp_size_rb);

  // Get UL-SCH information.
  ulsch_configuration ulsch_config;
  ulsch_config.tbs                   = units::bytes(data.size()).to_bits();
  ulsch_config.mcs_descr             = pdu.mcs_descr;
  ulsch_config.nof_harq_ack_bits     = units::bits(pdu.uci.nof_harq_ack);
  ulsch_config.nof_csi_part1_bits    = units::bits(pdu.uci.nof_csi_part1);
  ulsch_config.nof_csi_part2_bits    = 0_bits;
  ulsch_config.alpha_scaling         = pdu.uci.alpha_scaling;
  ulsch_config.beta_offset_harq_ack  = pdu.uci.beta_offset_harq_ack;
  ulsch_config.beta_offset_csi_part1 = pdu.uci.beta_offset_csi_part1;
  ulsch_config.beta_offset_csi_part2 = pdu.uci.beta_offset_csi_part2;
  ulsch_config.nof_rb                = nof_rb;
  ulsch_config.start_symbol_index    = pdu.start_symbol_index;
  ulsch_config.nof_symbols           = pdu.nof_symbols;
  ulsch_config.dmrs_type             = pdu.dmrs == dmrs_type::TYPE1 ? dmrs_config_type::type1 : dmrs_config_type::type2;
  ulsch_config.dmrs_symbol_mask      = pdu.dmrs_symbol_mask;
  ulsch_config.nof_cdm_groups_without_data = pdu.nof_cdm_groups_without_data;
  ulsch_config.nof_layers                  = pdu.nof_tx_layers;
  ulsch_information info                   = get_ulsch_information(ulsch_config);

  // Estimate channel.
  dmrs_pusch_estimator::configuration ch_est_config;
  ch_est_config.slot          = pdu.slot;
  ch_est_config.type          = pdu.dmrs;
  ch_est_config.scrambling_id = pdu.scrambling_id;
  ch_est_config.n_scid        = pdu.n_scid;
  ch_est_config.scaling       = convert_dB_to_amplitude(-get_sch_to_dmrs_ratio_dB(pdu.nof_cdm_groups_without_data));
  ch_est_config.c_prefix      = pdu.cp;
  ch_est_config.symbols_mask  = pdu.dmrs_symbol_mask;
  ch_est_config.rb_mask       = rb_mask;
  ch_est_config.first_symbol  = pdu.start_symbol_index;
  ch_est_config.nof_symbols   = pdu.nof_symbols;
  ch_est_config.nof_tx_layers = pdu.nof_tx_layers;
  ch_est_config.rx_ports.assign(pdu.rx_ports.begin(), pdu.rx_ports.end());
  depedencies.get_estimator().estimate(ch_estimate, grid, ch_est_config);

  // Handles the direct current if it is present.
  if (pdu.dc_position.has_value()) {
    unsigned dc_position = pdu.dc_position.value();
    for (unsigned i_port = 0, i_port_end = pdu.rx_ports.size(); i_port != i_port_end; ++i_port) {
      for (unsigned i_layer = 0, i_layer_end = pdu.nof_tx_layers; i_layer != i_layer_end; ++i_layer) {
        for (unsigned i_symbol = pdu.start_symbol_index, i_symbol_end = pdu.start_symbol_index + pdu.nof_symbols;
             i_symbol != i_symbol_end;
             ++i_symbol) {
          // Extract channel estimates for the OFDM symbol, port and layer.
          span<cf_t> ce = ch_estimate.get_symbol_ch_estimate(i_symbol, i_port, i_layer);

          // Set DC to zero.
          ce[dc_position] = 0;
        }
      }
    }
  }

  // Extract channel state information.
  channel_state_information csi(csi_sinr_calc_method);
  ch_estimate.get_channel_state_information(csi);

  // Prepare demultiplex configuration.
  ulsch_demultiplex::configuration demux_config;
  demux_config.modulation                  = pdu.mcs_descr.modulation;
  demux_config.nof_layers                  = pdu.nof_tx_layers;
  demux_config.nof_prb                     = nof_rb;
  demux_config.start_symbol_index          = pdu.start_symbol_index;
  demux_config.nof_symbols                 = pdu.nof_symbols;
  demux_config.nof_harq_ack_rvd            = info.nof_harq_ack_rvd.value();
  demux_config.dmrs                        = pdu.dmrs;
  demux_config.dmrs_symbol_mask            = ulsch_config.dmrs_symbol_mask;
  demux_config.nof_cdm_groups_without_data = ulsch_config.nof_cdm_groups_without_data;
  demux_config.nof_harq_ack_bits           = ulsch_config.nof_harq_ack_bits.value();
  demux_config.nof_enc_harq_ack_bits       = info.nof_harq_ack_bits.value();
  demux_config.nof_csi_part1_bits          = ulsch_config.nof_csi_part1_bits.value();
  demux_config.nof_enc_csi_part1_bits      = info.nof_csi_part1_bits.value();

  bool has_sch_data = pdu.codeword.has_value();

  // Prepare decoder buffers with dummy instances.
  std::reference_wrapper<pusch_decoder_buffer> decoder_buffer(decoder_buffer_dummy);
  std::reference_wrapper<pusch_decoder_buffer> harq_ack_buffer(decoder_buffer_dummy);
  std::reference_wrapper<pusch_decoder_buffer> csi_part1_buffer(decoder_buffer_dummy);

  // Prepare CSI Part 1 feedback.
  pusch_processor_csi_part1_feedback_impl csi_part1_feedback(depedencies.get_csi_part2_decoder(),
                                                             depedencies.get_demultiplex(),
                                                             pdu.mcs_descr.modulation,
                                                             pdu.uci.csi_part2_size,
                                                             ulsch_config);

  // Prepare notifiers.
  notifier_adaptor.new_transmission(notifier, csi_part1_feedback, csi);
  csi_part1_feedback.connect_notifier(notifier_adaptor);

  if (has_sch_data) {
    // Prepare decoder configuration.
    pusch_decoder::configuration decoder_config;
    decoder_config.base_graph          = pdu.codeword.value().ldpc_base_graph;
    decoder_config.rv                  = pdu.codeword.value().rv;
    decoder_config.mod                 = pdu.mcs_descr.modulation;
    decoder_config.Nref                = pdu.tbs_lbrm_bytes * 8;
    decoder_config.nof_layers          = pdu.nof_tx_layers;
    decoder_config.nof_ldpc_iterations = dec_nof_iterations;
    decoder_config.use_early_stop      = dec_enable_early_stop;
    decoder_config.new_data            = pdu.codeword.value().new_data;

    // Setup decoder.
    decoder_buffer =
        decoder->new_data(data, std::move(rm_buffer), notifier_adaptor.get_sch_data_notifier(), decoder_config);
  }

  // Prepares HARQ-ACK notifier and buffer.
  if (pdu.uci.nof_harq_ack != 0) {
    harq_ack_buffer = depedencies.get_harq_ack_decoder().new_transmission(
        pdu.uci.nof_harq_ack, pdu.mcs_descr.modulation, notifier_adaptor.get_harq_ack_notifier());
  }

  // Prepares CSI Part 1 notifier and buffer.
  if (pdu.uci.nof_csi_part1 != 0) {
    csi_part1_buffer = depedencies.get_csi_part1_decoder().new_transmission(
        pdu.uci.nof_csi_part1, pdu.mcs_descr.modulation, notifier_adaptor.get_csi_part1_notifier());
  }

  // Demultiplex SCH data, HARQ-ACK and CSI Part 1.
  pusch_codeword_buffer& demodulator_buffer =
      depedencies.get_demultiplex().demultiplex(decoder_buffer, harq_ack_buffer, csi_part1_buffer, demux_config);

  // Demodulate.
  pusch_demodulator::configuration demod_config;
  demod_config.rnti                        = pdu.rnti;
  demod_config.rb_mask                     = rb_mask;
  demod_config.modulation                  = pdu.mcs_descr.modulation;
  demod_config.start_symbol_index          = pdu.start_symbol_index;
  demod_config.nof_symbols                 = pdu.nof_symbols;
  demod_config.dmrs_symb_pos               = pdu.dmrs_symbol_mask;
  demod_config.dmrs_config_type            = pdu.dmrs;
  demod_config.nof_cdm_groups_without_data = pdu.nof_cdm_groups_without_data;
  demod_config.n_id                        = pdu.n_id;
  demod_config.nof_tx_layers               = pdu.nof_tx_layers;
  demod_config.rx_ports                    = pdu.rx_ports;
  depedencies.get_demodulator().demodulate(
      demodulator_buffer, notifier_adaptor.get_demodulator_notifier(), grid, ch_estimate, demod_config);
}

void pusch_processor_impl::assert_pdu(const pusch_processor::pdu_t& pdu, const channel_estimate& ch_estimate) const
{
  // Make sure the configuration is supported.
  srsran_assert((pdu.bwp_start_rb + pdu.bwp_size_rb) <= ch_estimate.capacity().nof_prb,
                "The sum of the BWP start (i.e., {}) and size (i.e., {}) exceeds the maximum grid size (i.e., {} PRB).",
                pdu.bwp_start_rb,
                pdu.bwp_size_rb,
                ch_estimate.capacity().nof_prb);
  srsran_assert(pdu.dmrs == dmrs_type::TYPE1, "Only DM-RS Type 1 is currently supported.");
  srsran_assert(pdu.nof_cdm_groups_without_data == 2, "Only two CDM groups without data are currently supported.");
  srsran_assert(
      pdu.nof_tx_layers <= ch_estimate.capacity().nof_tx_layers,
      "The number of transmit layers (i.e., {}) exceeds the maximum number of transmission layers (i.e., {}).",
      pdu.nof_tx_layers,
      ch_estimate.capacity().nof_tx_layers);
  srsran_assert(pdu.rx_ports.size() <= ch_estimate.capacity().nof_rx_ports,
                "The number of receive ports (i.e., {}) exceeds the maximum number of receive ports (i.e., {}).",
                pdu.rx_ports.size(),
                ch_estimate.capacity().nof_rx_ports);

  srsran_assert(pdu.uci.csi_part2_size.is_valid(pdu.uci.nof_csi_part1),
                "CSI Part 1 UCI field length (i.e., {}) does not correspond with the CSI Part 2 (i.e., {})",
                pdu.uci.nof_csi_part1,
                pdu.uci.csi_part2_size);

  // Check DC is whithin the CE.
  if (pdu.dc_position.has_value()) {
    interval<unsigned> dc_position_range(0, ch_estimate.size().nof_prb * NRE);
    srsran_assert(dc_position_range.contains(pdu.dc_position.value()),
                  "DC position (i.e., {}) is out of range {}.",
                  pdu.dc_position.value(),
                  dc_position_range);
  }
}
