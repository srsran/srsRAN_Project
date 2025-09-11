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

#include "pusch_processor_impl.h"
#include "pusch_decoder_buffer_dummy.h"
#include "pusch_processor_notifier_adaptor.h"
#include "pusch_processor_validator_impl.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc.h"
#include "srsran/phy/upper/channel_processors/pusch/formatters.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_codeword_buffer.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_buffer.h"
#include "srsran/phy/upper/unique_rx_buffer.h"
#include "srsran/ran/pusch/ulsch_info.h"
#include "srsran/ran/sch/sch_dmrs_power.h"
#include "srsran/ran/uci/uci_formatters.h"
#include "srsran/ran/uci/uci_part2_size_calculator.h"

using namespace srsran;

/// \brief Looks at the output of the validator and, if unsuccessful, fills \c msg with the error message.
///
/// This is used to call the validator inside the process methods only if asserts are active.
[[maybe_unused]] static bool handle_validation(std::string& msg, const error_type<std::string>& err)
{
  bool is_success = err.has_value();
  if (!is_success) {
    msg = err.error();
  }
  return is_success;
}

namespace {
class pusch_processor_csi_part1_feedback_impl : public pusch_processor_csi_part1_feedback
{
public:
  pusch_processor_csi_part1_feedback_impl(pusch_uci_decoder_wrapper&        csi_part2_decoder_,
                                          pusch_decoder&                    ulsch_decoder_,
                                          ulsch_demultiplex&                demultiplex_,
                                          modulation_scheme                 modulation_,
                                          const uci_part2_size_description& csi_part2_size_,
                                          const ulsch_configuration&        ulsch_config_) :
    csi_part2_decoder(csi_part2_decoder_),
    ulsch_decoder(ulsch_decoder_),
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

    // Set the number of UL-SCH softbits in the PUSCH decoder.
    ulsch_decoder.set_nof_softbits(info.nof_ul_sch_bits);
  }

private:
  pusch_processor_notifier_adaptor* notifier;
  pusch_uci_decoder_wrapper&        csi_part2_decoder;
  pusch_decoder&                    ulsch_decoder;
  ulsch_demultiplex&                demultiplex;
  modulation_scheme                 modulation;
  uci_part2_size_description        csi_part2_size;
  ulsch_configuration               ulsch_config;
};

} // namespace

// Dummy PUSCH decoder buffer. Used for PUSCH transmissions without SCH data.
static pusch_decoder_buffer_dummy decoder_buffer_dummy;

pusch_processor_impl::pusch_processor_impl(configuration& config) :
  logger(srslog::fetch_basic_logger("PHY")),
  thread_local_dependencies_pool(std::move(config.thread_local_dependencies_pool)),
  decoder(std::move(config.decoder)),
  dec_nof_iterations(config.dec_nof_iterations),
  force_decoding(config.dec_force_decoding),
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
  auto dependencies = thread_local_dependencies_pool->get();

  if (!dependencies) {
    logger.error("Failed to retrieve PUSCH processor dependencies.");

    // Notify
    if (pdu.uci.nof_harq_ack != 0) {
      notifier.on_uci({.harq_ack  = {.payload = uci_payload_type(pdu.uci.nof_harq_ack), .status = uci_status::invalid},
                       .csi_part1 = {},
                       .csi_part2 = {},
                       .csi       = {}});
    }

    // Notify the completion of the data processing as the CRC check is KO.
    if (pdu.codeword.has_value()) {
      notifier.on_sch({});
    }

    return;
  }

  // Get channel estimates.
  channel_estimate& ch_estimate = dependencies->get_channel_estimate();

  // Assert PDU.
  [[maybe_unused]] std::string msg;
  srsran_assert(
      handle_validation(msg, pusch_processor_validator_impl(ch_estimate.capacity()).is_valid(pdu)), "{}", msg);

  // Number of RB used by this transmission.
  unsigned nof_rb = pdu.freq_alloc.get_nof_rb();

  // Get RB mask relative to Point A. It assumes PUSCH is never interleaved.
  crb_bitmap rb_mask = pdu.freq_alloc.get_crb_mask(pdu.bwp_start_rb, pdu.bwp_size_rb);

  // Determine if the PUSCH allocation overlaps with the position of the DC.
  bool overlap_dc = false;
  if (pdu.dc_position.has_value()) {
    unsigned dc_position_prb = *pdu.dc_position / NRE;
    overlap_dc               = rb_mask.test(dc_position_prb);
  }

  bool      enable_transform_precoding  = false;
  unsigned  scrambling_id               = 0;
  unsigned  n_rs_id                     = 0;
  bool      n_scid                      = 0;
  unsigned  nof_cdm_groups_without_data = 2;
  dmrs_type dmrs_type                   = srsran::dmrs_type::TYPE1;
  if (std::holds_alternative<srsran::pusch_processor::dmrs_configuration>(pdu.dmrs)) {
    const auto& dmrs_config     = std::get<srsran::pusch_processor::dmrs_configuration>(pdu.dmrs);
    scrambling_id               = dmrs_config.scrambling_id;
    n_scid                      = dmrs_config.n_scid;
    nof_cdm_groups_without_data = dmrs_config.nof_cdm_groups_without_data;
    dmrs_type                   = dmrs_config.dmrs;
  } else {
    const auto& dmrs_config    = std::get<srsran::pusch_processor::dmrs_transform_precoding_configuration>(pdu.dmrs);
    enable_transform_precoding = true;
    n_rs_id                    = dmrs_config.n_rs_id;
  }

  // Get UL-SCH information as if there was no CSI Part 2 in the PUSCH.
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
  ulsch_config.dmrs_type        = (dmrs_type == dmrs_type::TYPE1 ? dmrs_config_type::type1 : dmrs_config_type::type2);
  ulsch_config.dmrs_symbol_mask = pdu.dmrs_symbol_mask;
  ulsch_config.nof_cdm_groups_without_data = nof_cdm_groups_without_data;
  ulsch_config.nof_layers                  = pdu.nof_tx_layers;
  ulsch_config.contains_dc                 = overlap_dc;
  ulsch_information info                   = get_ulsch_information(ulsch_config);

  // Estimate channel.
  dmrs_pusch_estimator::configuration ch_est_config;
  ch_est_config.slot = pdu.slot;
  if (enable_transform_precoding) {
    ch_est_config.sequence_config = dmrs_pusch_estimator::low_papr_sequence_configuration{.n_rs_id = n_rs_id};
  } else {
    ch_est_config.sequence_config = dmrs_pusch_estimator::pseudo_random_sequence_configuration{
        .type = dmrs_type, .nof_tx_layers = pdu.nof_tx_layers, .scrambling_id = scrambling_id, .n_scid = n_scid};
  }
  ch_est_config.scaling      = convert_dB_to_amplitude(-get_sch_to_dmrs_ratio_dB(nof_cdm_groups_without_data));
  ch_est_config.c_prefix     = pdu.cp;
  ch_est_config.symbols_mask = pdu.dmrs_symbol_mask;
  ch_est_config.rb_mask      = rb_mask;
  ch_est_config.first_symbol = pdu.start_symbol_index;
  ch_est_config.nof_symbols  = pdu.nof_symbols;
  ch_est_config.rx_ports.assign(pdu.rx_ports.begin(), pdu.rx_ports.end());
  dependencies->get_estimator().estimate(ch_estimate, grid, ch_est_config);

  // Set the DC (Direct Current) subcarrier to zero if its position is within the resource grid and transform precoding
  // is disabled. This step is skipped when transform precoding is used, as forcing the DC to zero in that case may
  // introduce non-linear distortion after the inverse transform. The issue is particularly pronounced for narrowband
  // PUSCH transmissions.
  if (pdu.dc_position.has_value() && std::holds_alternative<dmrs_configuration>(pdu.dmrs)) {
    for (unsigned i_port = 0, i_port_end = pdu.rx_ports.size(); i_port != i_port_end; ++i_port) {
      for (unsigned i_layer = 0, i_layer_end = pdu.nof_tx_layers; i_layer != i_layer_end; ++i_layer) {
        for (unsigned i_symbol = pdu.start_symbol_index, i_symbol_end = pdu.start_symbol_index + pdu.nof_symbols;
             i_symbol != i_symbol_end;
             ++i_symbol) {
          // Extract channel estimates for the OFDM symbol, port and layer.
          span<cbf16_t> ce = ch_estimate.get_symbol_ch_estimate(i_symbol, i_port, i_layer);

          // Set DC to zero.
          ce[*pdu.dc_position] = 0;
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
  demux_config.dmrs                        = dmrs_type;
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
  pusch_processor_csi_part1_feedback_impl csi_part1_feedback(dependencies->get_csi_part2_decoder(),
                                                             *decoder,
                                                             dependencies->get_demultiplex(),
                                                             pdu.mcs_descr.modulation,
                                                             pdu.uci.csi_part2_size,
                                                             ulsch_config);

  // Prepare notifiers.
  notifier_adaptor.new_transmission(notifier, csi_part1_feedback, csi);
  csi_part1_feedback.connect_notifier(notifier_adaptor);

  if (has_sch_data) {
    units::bits tbs            = units::bytes(data.size()).to_bits();
    unsigned    nof_codeblocks = ldpc::compute_nof_codeblocks(tbs, pdu.codeword->ldpc_base_graph);
    units::bits Nref           = ldpc::compute_N_ref(pdu.tbs_lbrm, nof_codeblocks);

    // Prepare decoder configuration.
    pusch_decoder::configuration decoder_config;
    decoder_config.base_graph          = pdu.codeword->ldpc_base_graph;
    decoder_config.rv                  = pdu.codeword->rv;
    decoder_config.mod                 = pdu.mcs_descr.modulation;
    decoder_config.Nref                = Nref.value();
    decoder_config.nof_layers          = pdu.nof_tx_layers;
    decoder_config.nof_ldpc_iterations = dec_nof_iterations;
    decoder_config.force_decoding      = force_decoding;
    decoder_config.use_early_stop      = dec_enable_early_stop;
    decoder_config.new_data            = pdu.codeword->new_data;

    // Setup decoder.
    decoder_buffer =
        decoder->new_data(data, std::move(rm_buffer), notifier_adaptor.get_sch_data_notifier(), decoder_config);

    // If there is no expected CSI Part 2 payload, the number of UL-SCH LLRs is known without the need to decode the
    // CSI Part 1 payload.
    if (pdu.uci.csi_part2_size.entries.empty()) {
      decoder->set_nof_softbits(info.nof_ul_sch_bits);
    }
  }

  // Prepares HARQ-ACK notifier and buffer.
  if (pdu.uci.nof_harq_ack != 0) {
    harq_ack_buffer = dependencies->get_harq_ack_decoder().new_transmission(
        pdu.uci.nof_harq_ack, pdu.mcs_descr.modulation, notifier_adaptor.get_harq_ack_notifier());
  }

  // Prepares CSI Part 1 notifier and buffer.
  if (pdu.uci.nof_csi_part1 != 0) {
    csi_part1_buffer = dependencies->get_csi_part1_decoder().new_transmission(
        pdu.uci.nof_csi_part1, pdu.mcs_descr.modulation, notifier_adaptor.get_csi_part1_notifier());
  }

  // Demultiplex SCH data, HARQ-ACK and CSI Part 1.
  pusch_codeword_buffer& demodulator_buffer =
      dependencies->get_demultiplex().demultiplex(decoder_buffer, harq_ack_buffer, csi_part1_buffer, demux_config);

  // Demodulate.
  pusch_demodulator::configuration demod_config;
  demod_config.rnti                        = pdu.rnti;
  demod_config.rb_mask                     = rb_mask;
  demod_config.modulation                  = pdu.mcs_descr.modulation;
  demod_config.start_symbol_index          = pdu.start_symbol_index;
  demod_config.nof_symbols                 = pdu.nof_symbols;
  demod_config.dmrs_symb_pos               = pdu.dmrs_symbol_mask;
  demod_config.dmrs_config_type            = dmrs_type;
  demod_config.nof_cdm_groups_without_data = nof_cdm_groups_without_data;
  demod_config.n_id                        = pdu.n_id;
  demod_config.nof_tx_layers               = pdu.nof_tx_layers;
  demod_config.enable_transform_precoding  = enable_transform_precoding;
  demod_config.rx_ports                    = pdu.rx_ports;
  dependencies->get_demodulator().demodulate(
      demodulator_buffer, notifier_adaptor.get_demodulator_notifier(), grid, ch_estimate, demod_config);
}
