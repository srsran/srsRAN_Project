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

#include "pdsch_processor_impl.h"
#include "pdsch_processor_helpers.h"
#include "pdsch_processor_validator_impl.h"
#include "srsran/srsvec/bit.h"

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

void pdsch_processor_impl::process(resource_grid_writer&                                           grid,
                                   pdsch_processor_notifier&                                       notifier,
                                   static_vector<shared_transport_block, MAX_NOF_TRANSPORT_BLOCKS> data,
                                   const pdsch_processor::pdu_t&                                   pdu)
{
  // Assert PDU.
  [[maybe_unused]] std::string msg;
  srsran_assert(handle_validation(msg, pdsch_processor_validator_impl().is_valid(pdu)), "{}", msg);

  // Number of layers from the precoding configuration.
  unsigned nof_layers = pdu.precoding.get_nof_layers();

  // Number of codewords.
  unsigned nof_codewords = (nof_layers > 4) ? 2 : 1;

  // Calculate the number of layers codeword 0 is mapped to. It is the number of layers divided by the number of
  // codewords, rounding down (floor).
  unsigned nof_layers_cw0 = nof_layers / nof_codewords;

  // Calculate the number of layers codeword 1 is mapped to. It is the unused number of layers from the previous
  // codeword.
  unsigned nof_layers_cw1 = nof_layers - nof_layers_cw0;

  // Calculate the number of resource elements used to map PDSCH on the grid. Common for all codewords.
  unsigned nof_re_pdsch = pdsch_compute_nof_data_re(pdu);

  // Prepare encoded codewords.
  static_vector<bit_buffer, pdsch_constants::MAX_NOF_CODEWORDS> codewords;

  // Encode each codeword.
  for (unsigned codeword_id = 0; codeword_id != nof_codewords; ++codeword_id) {
    unsigned          nof_layers_cw = (codeword_id == 0) ? nof_layers_cw0 : nof_layers_cw1;
    const bit_buffer& codeword = encode(data[codeword_id].get_buffer(), codeword_id, nof_layers_cw, nof_re_pdsch, pdu);

    codewords.emplace_back(codeword);
  }

  // Modulate codewords.
  modulate(grid, codewords, pdu);

  if (pdu.ptrs) {
    // Prepare PT-RS configuration and generate.
    pdsch_process_ptrs(grid, *ptrs, pdu);
  }

  // Prepare DM-RS configuration and generate.
  pdsch_process_dmrs(grid, *dmrs, pdu);

  // Notify the end of the processing.
  notifier.on_finish_processing();
}

const bit_buffer& pdsch_processor_impl::encode(span<const uint8_t> data,
                                               unsigned            codeword_id,
                                               unsigned            nof_layers,
                                               unsigned            Nre,
                                               const pdu_t&        pdu)
{
  // Select codeword specific parameters.
  unsigned          rv         = pdu.codewords[codeword_id].rv;
  modulation_scheme modulation = pdu.codewords[codeword_id].modulation;

  span<uint8_t> tmp_codeword = temp_unpacked_codeword;

  // Calculate rate match buffer size.
  units::bits Nref = ldpc::compute_N_ref(
      pdu.tbs_lbrm, ldpc::compute_nof_codeblocks(units::bytes(data.size()).to_bits(), pdu.ldpc_base_graph));

  // Prepare encoder configuration.
  pdsch_encoder::configuration encoder_config;
  encoder_config.base_graph     = pdu.ldpc_base_graph;
  encoder_config.rv             = rv;
  encoder_config.mod            = modulation;
  encoder_config.Nref           = Nref.value();
  encoder_config.nof_layers     = nof_layers;
  encoder_config.nof_ch_symbols = Nre * nof_layers;

  // Prepare codeword size.
  span<uint8_t> codeword = tmp_codeword.first(Nre * nof_layers * get_bits_per_symbol(modulation));

  // Encode codeword.
  encoder->encode(codeword, data, encoder_config);

  // Pack encoded bits.
  temp_packed_codewords[codeword_id].resize(codeword.size());
  srsvec::bit_pack(temp_packed_codewords[codeword_id], codeword);

  // Return the view of the codeword.
  return temp_packed_codewords[codeword_id];
}

void pdsch_processor_impl::modulate(resource_grid_writer& grid, span<const bit_buffer> codewords, const pdu_t& pdu)
{
  unsigned nof_codewords = codewords.size();

  pdsch_modulator::config_t modulator_config;
  modulator_config.rnti            = pdu.rnti;
  modulator_config.bwp             = {pdu.bwp_start_rb, pdu.bwp_start_rb + pdu.bwp_size_rb};
  modulator_config.modulation1     = pdu.codewords[0].modulation;
  modulator_config.modulation2     = nof_codewords > 1 ? pdu.codewords[1].modulation : modulation_scheme::BPSK;
  modulator_config.freq_allocation = pdu.freq_alloc;
  modulator_config.time_alloc      = {pdu.start_symbol_index, pdu.start_symbol_index + pdu.nof_symbols};
  modulator_config.dmrs_symb_pos   = pdu.dmrs_symbol_mask;
  modulator_config.nof_cdm_groups_without_data = pdu.nof_cdm_groups_without_data;
  modulator_config.n_id                        = pdu.n_id;
  modulator_config.scaling                     = convert_dB_to_amplitude(-pdu.ratio_pdsch_data_to_sss_dB);
  modulator_config.reserved                    = pdu.reserved;
  modulator_config.precoding                   = pdu.precoding;

  modulator->modulate(grid, codewords, modulator_config);
}
